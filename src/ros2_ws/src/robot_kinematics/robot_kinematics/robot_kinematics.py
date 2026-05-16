import os  
import rclpy
from rclpy.node import Node
import pybullet as p
import math
import time
import numpy as np
from tf2_ros import TransformBroadcaster
from geometry_msgs.msg import TransformStamped
from tf2_ros.static_transform_broadcaster import StaticTransformBroadcaster
from sensor_msgs.msg import Image,CameraInfo
from robot_interfaces.srv import SolveIK, AddObstacle, RemoveObstacle, SolveDK, SpawnObject,AddCamera
from ament_index_python.packages import get_package_share_directory
from ament_index_python.packages import PackageNotFoundError
from std_msgs.msg import String
import json

class PyBulletKinematics(Node):
  def __init__(self):
    super().__init__('ik_server_node')
    
    self.srv_ik = self.create_service(SolveIK, 'solve_ik', self.ik_callback)
    self.srv_dk = self.create_service(SolveDK, 'solve_dk', self.dk_callback)
    self.srv_add = self.create_service(AddObstacle, 'add_wall', self.add_wall_callback)
    self.srv_rem = self.create_service(RemoveObstacle, 'remove_wall', self.remove_wall_callback)
    
    self.sync_pub = self.create_publisher(String, '/env_sync', 10)

    self.walls = {}
    self.enviroment_objects = {}
    self.cameras_config = {}
    self.camera_publishers = {}
    self.camera_timer = None
    self.physicsClient = p.connect(p.GUI)
    
    # Configuración del Robot
    pkg_path = get_package_share_directory('robot_pkg') 
    urdf_path = os.path.join(pkg_path, 'urdf', 'robot.urdf')
    
    if not os.path.exists(urdf_path):
      self.get_logger().error(f"¡URDF no encontrado en: {urdf_path}!")
    
    self.robot_id = p.loadURDF(urdf_path, useFixedBase=True)
    self.end_effector_index = 5
    
    # Identificar articulaciones móviles
    self.movable_joints = []
    for i in range(p.getNumJoints(self.robot_id)):
      joint_info = p.getJointInfo(self.robot_id, i)
      if joint_info[2] != p.JOINT_FIXED:
        self.movable_joints.append(i)
            
    
    init_pos = p.getLinkState(self.robot_id, self.end_effector_index)[4]
    self.debug_text_id = p.addUserDebugText(
      text=f"X: {init_pos[0]:.2f} Y: {init_pos[1]:.2f} Z: {init_pos[2]:.2f}",
      textPosition=[init_pos[0], init_pos[1], init_pos[2] + 0.1],
      textColorRGB=[1, 0, 0], textSize=1.5
    )
    self.debug_line_id = -1

    self.tf_broadcaster = TransformBroadcaster(self)
    
    self.get_logger().info('Servidor IK y Obstáculos listo.')

  def ik_callback(self, request, response):
      
    target_pos = [request.target_pose.position.x, 
                  request.target_pose.position.y, 
                  request.target_pose.position.z]
    target_orn = [request.target_pose.orientation.x, 
                  request.target_pose.orientation.y, 
                  request.target_pose.orientation.z, 
                  request.target_pose.orientation.w]

    try:
      if len(request.origin_joint_angles) != len(self.movable_joints):
        self.get_logger().error("El número de ángulos de origen no coincide con las articulaciones del robot.")
        response.success = False
        return response
        
      #saved_joint_states = [p.getJointState(self.robot_id, j)[0] for j in self.movable_joints]
      saved_joint_states = list(request.origin_joint_angles)
      for i, joint_idx in enumerate(self.movable_joints):
        p.resetJointState(self.robot_id, joint_idx, saved_joint_states[i])
      

      if not self.update_tool_collision(request):
        response.success = False
        return response
      
      tcp_pos, tcp_orn = self.get_tcp_info(request.tcp_offset, request.tool_dimensions)

      angles_matered=True
      if all(angle == 0 for angle in target_orn):
        angles_matered=False
        _, curr_wrist_orn = p.getLinkState(self.robot_id, self.end_effector_index)[4:6]
        _, curr_tip_orn = p.multiplyTransforms([0,0,0], curr_wrist_orn, [0,0,0], tcp_orn)
        
        target_orn = curr_tip_orn

      inv_pos, inv_orn = p.invertTransform(tcp_pos, tcp_orn)
      
      # Muñeca = Target * TCP_Invertido
      wrist_target_pos, wrist_target_orn = p.multiplyTransforms(
        target_pos, target_orn, inv_pos, inv_orn
      )
      self.get_logger().info( f'Muneca destino: X={wrist_target_pos[0]:.3f}, Y={wrist_target_pos[1]:.3f}, Z={wrist_target_pos[2]:.3f}')
      self.get_logger().info( f'Muneca destino orinetacion: X={wrist_target_orn[0]:.3f}, Y={wrist_target_orn[1]:.3f}, Z={wrist_target_orn[2]:.3f}')

      if (not angles_matered):
        self.get_logger().info(f'No importa orientacion')
        joint_poses = p.calculateInverseKinematics(self.robot_id, self.end_effector_index, wrist_target_pos)
      else:
        self.get_logger().info(f'Si importa orientacion')
        joint_poses = p.calculateInverseKinematics(self.robot_id, self.end_effector_index, wrist_target_pos, wrist_target_orn)
      
      if any(math.isnan(ang) for ang in joint_poses):
        self.get_logger().error("IK devolvió NaN. Objetivo inalcanzable.")
        response.success = False
        return response
      
      curr_wrist_pos = p.getLinkState(self.robot_id, self.end_effector_index)[4]
      dist_cartesiana = math.sqrt(sum((a - b)**2 for a, b in zip(curr_wrist_pos, wrist_target_pos)))

      if dist_cartesiana < 0.04:
        UMBRAL_SINGULARIDAD_RAD = 3 
        es_singularidad = False
        
        for i, target_angle in enumerate(joint_poses):
          current_angle = saved_joint_states[i]
          salto = abs(target_angle - current_angle)
          
          if salto > UMBRAL_SINGULARIDAD_RAD:
            self.get_logger().error(
              f"¡SINGULARIDAD DETECTADA! La articulación {self.movable_joints[i]} "
              f"intenta dar un latigazo de {salto:.2f} rads para un avance cartesiano de solo {dist_cartesiana:.3f}m."
            )
            es_singularidad = True
            break
                
        #  Si es singularidad, ABORTAR ANTES de mover el robot 
        if es_singularidad:
          response.success = False
          response.joint_angles = []
          return response
      
      for i, joint_idx in enumerate(self.movable_joints):
        p.resetJointState(self.robot_id, joint_idx, joint_poses[i])

      new_wrist_state = p.getLinkState(self.robot_id, self.end_effector_index)
      new_tool_pos, new_tool_orn = p.multiplyTransforms(
        new_wrist_state[4], new_wrist_state[5], 
        self.tool_local_pos, 
        self.tool_local_orn
      )
      p.resetBasePositionAndOrientation(self.tool_body_id, new_tool_pos, new_tool_orn)

      p.stepSimulation()

      wrist_state = p.getLinkState(self.robot_id, self.end_effector_index)
      actual_tip_pos, _ = p.multiplyTransforms(wrist_state[4], wrist_state[5], tcp_pos, tcp_orn)
      w_pos = wrist_state[4] 

      self.get_logger().info(
        f'Muneca real: X={w_pos[0]:.3f}, Y={w_pos[1]:.3f}, Z={w_pos[2]:.3f} | '
        f'Punta real: X={actual_tip_pos[0]:.3f}, Y={actual_tip_pos[1]:.3f}, Z={actual_tip_pos[2]:.3f}'
      )
      
      #COLISIONES (Robot + Herramienta)
      colision = False
      wall_name_colision = ''
      p.performCollisionDetection()
      
      for name, wall_id in self.walls.items():
        # Contactos con el robot
        contacts_robot = p.getContactPoints(bodyA=self.robot_id, bodyB=wall_id)
        # Contactos con la herramienta
        contacts_tool = p.getContactPoints(bodyA=self.tool_body_id, bodyB=wall_id)
        
        if len(contacts_robot) > 0 or len(contacts_tool) > 0:
          colision = True
          wall_name_colision = name  
          break

      error_dist = math.sqrt(sum((a - b)**2 for a, b in zip(actual_tip_pos, target_pos)))
      tolerancia = 0.05 ############
      
      if error_dist > tolerancia or colision:
        self.update_debug_text(actual_tip_pos)
        if colision:
          self.get_logger().warn(f'¡COLISIÓN detectada con pared: {wall_name_colision}!')
        else:
          self.get_logger().warn(f'Meta inalcanzable (Error: {error_dist:.3f}m)')
        
        time.sleep(10) #############################################################################################################################
        # REBOBINAR
        for i, joint_idx in enumerate(self.movable_joints):
          p.resetJointState(self.robot_id, joint_idx, saved_joint_states[i])

        restored_wrist_state = p.getLinkState(self.robot_id, self.end_effector_index)
        restored_tool_pos, restored_tool_orn = p.multiplyTransforms(
          restored_wrist_state[4], restored_wrist_state[5], 
          self.tool_local_pos, 
          self.tool_local_orn
        )
        p.resetBasePositionAndOrientation(self.tool_body_id, restored_tool_pos, restored_tool_orn)
        restored_tip_pos, _ = p.multiplyTransforms(restored_wrist_state[4], restored_wrist_state[5], tcp_pos, tcp_orn)
        self.update_debug_text(restored_tip_pos)

        response.success = False
        response.joint_angles = []
      else:
        self.get_logger().info(f'IK Exitosa. Error punta: {error_dist:.4f}m')
        self.update_debug_text(actual_tip_pos)
        
        response.joint_angles = [joint_poses[i] for i in range(len(self.movable_joints))]
        response.success = True

    except Exception as e:
      self.get_logger().error(f'Error crítico en IK: {str(e)}')
      response.success = False

    if hasattr(self, 'debug_line_id') and self.debug_line_id >= 0:
      p.removeUserDebugItem(self.debug_line_id)
    self.debug_line_id = p.addUserDebugLine(target_pos, [target_pos[0]+0.1, target_pos[1], target_pos[2]], [1,0,0], 5)
    
    return response
  
  def dk_callback(self, request, response):
    try:
      angles = request.joint_angles
      old_angles = [p.getJointState(self.robot_id, i)[0] for i in self.movable_joints]

      for i, joint_idx in enumerate(self.movable_joints):
        p.resetJointState(self.robot_id, joint_idx, angles[i])

      w_pos, w_orn = p.getLinkState(self.robot_id, self.end_effector_index)[4:6]
      tcp_pos, tcp_orn = self.get_tcp_info(request.tcp_offset, request.tool_dimensions)
      
      #Punta = Muñeca * TCP
      f_pos, f_orn = p.multiplyTransforms(w_pos, w_orn, tcp_pos, tcp_orn)

      for i, joint_idx in enumerate(self.movable_joints):
        p.resetJointState(self.robot_id, joint_idx, old_angles[i])

      response.target_pose.position.x, response.target_pose.position.y, response.target_pose.position.z = f_pos
      response.target_pose.orientation.x, response.target_pose.orientation.y, response.target_pose.orientation.z, response.target_pose.orientation.w = f_orn
      response.success = True

    except Exception as e:
      self.get_logger().error(f"Error en FK: {str(e)}")
      response.success = False
    
    return response

  def get_tcp_info(self, tcp_offset, tool_dimensions):
    largo = tool_dimensions[0]
    
    #  Muñeca a la BASE de la herramienta
    base_pos = [tcp_offset.position.x, tcp_offset.position.y, tcp_offset.position.z]
    base_orn = [tcp_offset.orientation.x, tcp_offset.orientation.y, 
                tcp_offset.orientation.z, tcp_offset.orientation.w]
    if all(v == 0 for v in base_orn): base_orn = [0, 0, 0, 1]

    # Base al EXTREMO (Punta)
    tip_local_pos = [0, 0, largo]
    tip_local_orn = [0, 0, 0, 1]

    # PUNTA respecto a la MUÑECA
    tcp_pos, tcp_orn = p.multiplyTransforms(base_pos, base_orn, tip_local_pos, tip_local_orn)
    norm = math.sqrt(sum([v**2 for v in tcp_orn]))
    if norm > 0:
      tcp_orn = [v/norm for v in tcp_orn]
    
    return tcp_pos, tcp_orn
  

  # --- MÉTODOS DE GESTIÓN DE OBSTÁCULOS  ---

  def add_wall_callback(self, request, response):
    try:
      if request.name in self.walls:
        p.removeBody(self.walls[request.name])

      half_extents = [request.width/2, request.depth/2, request.height/2]
      col_id = p.createCollisionShape(p.GEOM_BOX, halfExtents=half_extents)
      vis_id = p.createVisualShape(p.GEOM_BOX, halfExtents=half_extents, rgbaColor=[1, 0, 0, 0.4])
      
      # --- MAGIA DE ROTACIÓN AQUÍ ---
      # Convertimos Roll, Pitch, Yaw a Cuaternión
      orientation_q = p.getQuaternionFromEuler([request.roll, request.pitch, request.yaw])
      
      wall_id = p.createMultiBody(
        baseMass=0,
        baseCollisionShapeIndex=col_id,
        baseVisualShapeIndex=vis_id,
        basePosition=[request.x, request.y, request.z],
        baseOrientation=orientation_q # Añadimos la orientación al cuerpo
      )
      
      self.walls[request.name] = wall_id 
      self.get_logger().info(f"Pared '{request.name}' activa con rotación.")
      response.success = True
      
      # Actualizamos el visualizador JSON
      wall_info = {
          "method": "add_wall",
          "name": request.name,
          "x": request.x,
          "y": request.y,
          "z": request.z,
          "roll": request.roll,
          "pitch": request.pitch,
          "yaw": request.yaw,
          "width": request.width,
          "depth": request.depth,
          "height": request.height
      }
      self.sync_pub.publish(String(data=json.dumps(wall_info)))

    except Exception as e:
      self.get_logger().error(f"Error AddWall: {str(e)}")
      response.success = False
      
    return response

  def remove_wall_callback(self, request, response):
    if request.name in self.walls:
      p.removeBody(self.walls.pop(request.name))
      self.get_logger().info(f"Pared '{request.name}' eliminada.")
      sync_info = {
            "method": "remove_wall",
            "name": request.name
        }
      self.sync_pub.publish(String(data=json.dumps(sync_info)))
      response.success = True
    else:
      response.success = False
    return response

  def update_debug_text(self, pos):
    self.debug_text_id = p.addUserDebugText(
      text=f"X: {pos[0]:.2f}\nY: {pos[1]:.2f}\nZ: {pos[2]:.2f}",
      textPosition=[pos[0], pos[1], pos[2] + 0.1],
      textColorRGB=[1, 0, 0],
      textSize=1.2,
      replaceItemUniqueId=self.debug_text_id
    )

  def update_tool_collision(self, request):
    tcp = request.tcp_offset
    dims = request.tool_dimensions 
    t_type = request.tool_type

    if t_type not in [0, 1]:
      self.get_logger().error(f"Tipo de herramienta inválido: {t_type}. Solo se permite 0 (Caja) o 1 (Cilindro).")
      return False 
    
    if len(dims) < 2:
      self.get_logger().error("Dimensiones de herramienta insuficientes.")
      return False

    if hasattr(self, 'tool_body_id'):
      p.removeBody(self.tool_body_id)

    try:
      if t_type == 0: # CAJA
        # dims = [largo(z), ancho(x), profundo(y)]
        half_extents = [dims[1]/2, dims[2]/2, dims[0]/2]
        col_id = p.createCollisionShape(p.GEOM_BOX, halfExtents=half_extents)
        vis_id = p.createVisualShape(p.GEOM_BOX, halfExtents=half_extents, rgbaColor=[0.7, 0.7, 0.7, 1])
        z_offset = dims[0] / 2
      
      elif t_type == 1: # CILINDRO
        # dims = [largo(height), radio]
        col_id = p.createCollisionShape(p.GEOM_CYLINDER, radius=dims[1], height=dims[0])
        vis_id = p.createVisualShape(p.GEOM_CYLINDER, radius=dims[1], length=dims[0], rgbaColor=[0.8, 0.8, 0.8, 1])
        z_offset = dims[0] / 2

      #  muñeca AHORA MISMO
      wrist_state = p.getLinkState(self.robot_id, self.end_effector_index)
      w_pos = wrist_state[4]
      w_orn = wrist_state[5]
      spawn_pos, spawn_orn = p.multiplyTransforms(
        w_pos, w_orn, 
        [tcp.position.x, tcp.position.y, tcp.position.z + z_offset + 0.04], 
        [tcp.orientation.x, tcp.orientation.y, tcp.orientation.z, tcp.orientation.w]
      )
      self.tool_local_pos = [tcp.position.x, tcp.position.y, tcp.position.z + z_offset + 0.04]
      self.tool_local_orn = [tcp.orientation.x, tcp.orientation.y, tcp.orientation.z, tcp.orientation.w]

      self.tool_body_id = p.createMultiBody(
        baseMass=0.1, 
        baseCollisionShapeIndex=col_id, 
        baseVisualShapeIndex=vis_id,
        basePosition=spawn_pos,       
        baseOrientation=spawn_orn     
      )

      self.get_logger().info(f"Nuevo objeto herramienta creado con ID: {self.tool_body_id}")

      # Auto-colisión
      for j in range(-1, p.getNumJoints(self.robot_id)):
        p.setCollisionFilterPair(self.robot_id, self.tool_body_id, j, -1, enableCollision=0)

      # 4. Atar la herramienta al robot
      p.createConstraint(
        parentBodyUniqueId=self.robot_id,
        parentLinkIndex=self.end_effector_index,
        childBodyUniqueId=self.tool_body_id,
        childLinkIndex=-1,
        jointType=p.JOINT_FIXED,
        jointAxis=[0, 0, 0],
        parentFramePosition=[tcp.position.x, tcp.position.y, tcp.position.z + z_offset + 0.04],
        childFramePosition=[0, 0, 0],
        parentFrameOrientation=[tcp.orientation.x, tcp.orientation.y, tcp.orientation.z, tcp.orientation.w]
      )
      
      return True

    except Exception as e:
      self.get_logger().error(f"Error al crear la geometría de la herramienta: {str(e)}")
      return False
  
  

def main(args=None):
  rclpy.init(args=args)
  node = PyBulletKinematics()
  try:
    rclpy.spin(node)
  except KeyboardInterrupt:
    pass
  finally:
    p.disconnect()
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
  main()