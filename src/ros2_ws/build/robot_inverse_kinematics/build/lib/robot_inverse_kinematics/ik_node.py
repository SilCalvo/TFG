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

class PyBulletIKServer(Node):
  def __init__(self):
    super().__init__('ik_server_node')
    
    self.srv_ik = self.create_service(SolveIK, 'solve_ik', self.ik_callback)
    self.srv_dk = self.create_service(SolveDK, 'solve_dk', self.dk_callback)
    self.srv_add = self.create_service(AddObstacle, 'add_wall', self.add_wall_callback)
    self.srv_rem = self.create_service(RemoveObstacle, 'remove_wall', self.remove_wall_callback)
    self.srv_spawn = self.create_service(SpawnObject, 'add_object', self.spawn_object_callback)
    self.srv_camera = self.create_service(AddCamera, 'add_camera', self.add_camera_callback)
    self.srv_remove_object = self.create_service(SpawnObject, 'remove_object', self.remove_object_callback)
    self.srv_remove_camera = self.create_service(AddCamera, 'remove_camera', self.remove_camera_callback)

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
      saved_joint_states = [p.getJointState(self.robot_id, j)[0] for j in self.movable_joints]

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
        UMBRAL_SINGULARIDAD_RAD = 0.5  # Aprox 28 grados de límite por motor
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
      
      wall_id = p.createMultiBody(
        baseMass=0,
        baseCollisionShapeIndex=col_id,
        baseVisualShapeIndex=vis_id,
        basePosition=[request.x, request.y, request.z]
      )
      
      self.walls[request.name] = wall_id 
      self.get_logger().info(f"Pared '{request.name}' activa.")
      response.success = True
    except Exception as e:
      self.get_logger().error(f"Error AddWall: {str(e)}")
      response.success = False
    return response

  def remove_wall_callback(self, request, response):
    if request.name in self.walls:
      p.removeBody(self.walls.pop(request.name))
      self.get_logger().info(f"Pared '{request.name}' eliminada.")
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
  
  
  def spawn_object_callback(self, request, response):
    try:
      if request.name in self.enviroment_objects:
        p.removeBody(self.enviroment_objects[request.name])

      try:
        pkg_path = get_package_share_directory(request.package_name)
      except PackageNotFoundError:
        self.get_logger().error(f"¡Rechazado! El paquete '{request.package_name}' no existe en tu workspace.")
        response.success = False
        return response
      
      urdf_name = f"{request.piece_type.lower()}.urdf"
      urdf_file_path = os.path.join(pkg_path, 'urdf', urdf_name)
      
      if not os.path.exists(urdf_file_path):
        self.get_logger().error(f"¡Rechazado! No se encuentra el archivo URDF en: {urdf_file_path}")
        response.success = False
        return response

      piece_id = p.loadURDF(
        urdf_file_path, 
        basePosition=[request.x, request.y, request.z],
        useFixedBase=True 
      )
      
      self.enviroment_objects[request.name] = piece_id
      self.get_logger().info(f"Objeto '{urdf_name}' colocado en {request.x:.2f}, {request.y:.2f}, {request.z:.2f}.")
      response.success = True

    except Exception as e:
      self.get_logger().error(f"Error al cargar el URDF dinámico: {str(e)}")
      response.success = False

    return response
  



  def add_camera_callback(self, request, response):
    try:
        fov = 60.0
        near = 0.01
        far = 10.0

        view_matrix = p.computeViewMatrix(
            cameraEyePosition=[request.x, request.y, request.z],
            cameraTargetPosition=[request.target_x, request.target_y, request.target_z],
            cameraUpVector=[0, 0, 1]
        )
        proj_matrix = p.computeProjectionMatrixFOV(
            fov=fov, 
            aspect=float(request.width)/float(request.height), 
            nearVal=near, 
            farVal=far
        )

        # CÁLCULO DE INTRÍNSECAS (Modelo Pinhole)
        fov_rad = math.radians(fov)
        f_y = (request.height / 2.0) / math.tan(fov_rad / 2.0)
        f_x = f_y  # Asumimos píxeles cuadrados
        c_x = request.width / 2.0
        c_y = request.height / 2.0

        self.cameras_config[request.name] = {
            'view': view_matrix,
            'proj': proj_matrix,
            'width': request.width,
            'height': request.height,
            'near': near,
            'far': far,
            'fx': f_x, 'fy': f_y, 'cx': c_x, 'cy': c_y
        }

        # Creamos los 3 publicadores estándar de una cámara ROS 2
        if request.name not in self.camera_publishers:
            self.camera_publishers[request.name] = {
                'rgb': self.create_publisher(Image, f'/camera/{request.name}/color/image_raw', 10),
                'depth': self.create_publisher(Image, f'/camera/{request.name}/depth/image_raw', 10),
                'info': self.create_publisher(CameraInfo, f'/camera/{request.name}/camera_info', 10)
            }

        if self.camera_timer is None:
            self.get_logger().info("Detectada primera cámara. Activando sistema de visión RGB-D (10Hz)...")
            self.camera_timer = self.create_timer(0.1, self.publish_camera_images)

        self.get_logger().info(f"Cámara RGB-D '{request.name}' configurada correctamente.")
        
        pos = np.array([request.x, request.y, request.z])
        target = np.array([request.target_x, request.target_y, request.target_z])
        forward = (target - pos)
        forward = forward / np.linalg.norm(forward)
        
        world_up = np.array([0.0, 0.0, 1.0])
        right = np.cross(forward, world_up)
        right = right / np.linalg.norm(right)
        down = np.cross(forward, right)
        
        rot_matrix = np.array([
            [right[0], down[0], forward[0]],
            [right[1], down[1], forward[1]],
            [right[2], down[2], forward[2]]
        ])
        
        tr = np.trace(rot_matrix)
        if tr > 0:
            S = np.sqrt(tr + 1.0) * 2
            qw = 0.25 * S
            qx = (rot_matrix[2, 1] - rot_matrix[1, 2]) / S
            qy = (rot_matrix[0, 2] - rot_matrix[2, 0]) / S
            qz = (rot_matrix[1, 0] - rot_matrix[0, 1]) / S
        else:
            qx, qy, qz, qw = 0.0, 0.707, 0.0, 0.707 

        # En vez de publicar, lo GUARDAMOS en la configuración de esta cámara
        self.cameras_config[request.name] = {
            'view': view_matrix,
            'proj': proj_matrix,
            'width': request.width,
            'height': request.height,
            'near': near,
            'far': far,
            'fx': f_x, 'fy': f_y, 'cx': c_x, 'cy': c_y,
            'pos': [request.x, request.y, request.z], # <-- Guardamos Posición
            'quat': [qx, qy, qz, qw]                  # <-- Guardamos Rotación
        }
        response.success = True

    except Exception as e:
        self.get_logger().error(f"Error al añadir cámara: {str(e)}")
        response.success = False
    return response
  

  def publish_camera_images(self):
    if not self.cameras_config:
        self.get_logger().info("No hay cámaras. Apagando sistema de visión.")
        self.destroy_timer(self.camera_timer)
        self.camera_timer = None
        return

    for name, config in self.cameras_config.items():
        try:
            # Captura de imagen completa (Color + Profundidad)
            w, h, rgb_img, depth_buffer, _ = p.getCameraImage(
                width=config['width'],
                height=config['height'],
                viewMatrix=config['view'],
                projectionMatrix=config['proj'],
                renderer=p.ER_BULLET_HARDWARE_OPENGL
            )

            stamp = self.get_clock().now().to_msg()
            frame_id = f"camera_{name}_link"

            msg_rgb = Image()
            msg_rgb.header.stamp = stamp
            msg_rgb.header.frame_id = frame_id
            msg_rgb.height = h
            msg_rgb.width = w
            msg_rgb.encoding = "rgba8"
            msg_rgb.step = w * 4
            msg_rgb.data = np.array(rgb_img, dtype=np.uint8).tobytes()
            self.camera_publishers[name]['rgb'].publish(msg_rgb)

            depth_np = np.array(depth_buffer, dtype=np.float32)
            far = config['far']
            near = config['near']
            depth_real_meters = (far * near) / (far - (far - near) * depth_np)

            msg_depth = Image()
            msg_depth.header.stamp = stamp
            msg_depth.header.frame_id = frame_id
            msg_depth.height = h
            msg_depth.width = w
            msg_depth.encoding = "32FC1" # 32-bit Float, 1 Canal (Metros)
            msg_depth.is_bigendian = False
            msg_depth.step = w * 4
            msg_depth.data = depth_real_meters.tobytes()
            self.camera_publishers[name]['depth'].publish(msg_depth)

            msg_info = CameraInfo()
            msg_info.header.stamp = stamp
            msg_info.header.frame_id = frame_id
            msg_info.height = h
            msg_info.width = w
            msg_info.distortion_model = "plumb_bob"
            msg_info.d = [0.0, 0.0, 0.0, 0.0, 0.0] # Lente perfecta sin distorsión
            
            # K = [fx, 0, cx, 0, fy, cy, 0, 0, 1]
            msg_info.k = [config['fx'], 0.0, config['cx'], 
                          0.0, config['fy'], config['cy'], 
                          0.0, 0.0, 1.0]
            
            # P = Matriz de proyección rectificada
            msg_info.p = [config['fx'], 0.0, config['cx'], 0.0,
                          0.0, config['fy'], config['cy'], 0.0,
                          0.0, 0.0, 1.0, 0.0]
            
            self.camera_publishers[name]['info'].publish(msg_info)
            
            # --- 1. PUBLICACIÓN DEL LINK FRAME (Z arriba, X frente) ---
            t_link = TransformStamped()
            t_link.header.stamp = stamp
            t_link.header.frame_id = 'base' # Cambia a 'base_link' si prefieres
            t_link.child_frame_id = frame_id # camera_camara_tablero_link
            
            t_link.transform.translation.x = config['pos'][0]
            t_link.transform.translation.y = config['pos'][1]
            t_link.transform.translation.z = config['pos'][2]
            t_link.transform.rotation.x = config['quat'][0]
            t_link.transform.rotation.y = config['quat'][1]
            t_link.transform.rotation.z = config['quat'][2]
            t_link.transform.rotation.w = config['quat'][3]
            
            # --- 2. PUBLICACIÓN DEL OPTICAL FRAME (Z frente, Y abajo) ---
            t_opt = TransformStamped()
            t_opt.header.stamp = stamp
            t_opt.header.frame_id = frame_id # El padre es el Link Frame
            t_opt.child_frame_id = f"camera_{name}_optical_frame"
            
            # Rotación mágica para pasar de convención de mundo a convención de cámara (OpenCV)
            # Rota -90º en X y 90º en Z
            t_opt.transform.rotation.x = 0.0
            t_opt.transform.rotation.y = 0.7071
            t_opt.transform.rotation.z = 0.0
            t_opt.transform.rotation.w = 0.7071
            # (La traslación es 0, ya que coinciden en el mismo punto físico)

            # Publicamos ambas transformaciones de forma dinámica
            self.tf_broadcaster.sendTransform([t_link, t_opt])

        except Exception as e:
            self.get_logger().error(f"Error en streaming RGB-D de {name}: {str(e)}")

  def remove_object_callback(self, request, response):
    if request.name in self.enviroment_objects:
      try:
        p.removeBody(self.enviroment_objects[request.name])
        del self.enviroment_objects[request.name]
        
        self.get_logger().info(f"Objeto '{request.name}' eliminado del entorno.")
        response.success = True
      except Exception as e:
        self.get_logger().error(f"Error al borrar el objeto '{request.name}': {str(e)}")
        response.success = False
    else:
      self.get_logger().warn(f"No se pudo borrar: El objeto '{request.name}' no existe.")
      response.success = False
      
    return response
  

  def remove_camera_callback(self, request, response):
    if request.name in self.cameras_config:
      try:
        del self.cameras_config[request.name]
        
        if request.name in self.camera_publishers:
          for pub in self.camera_publishers[request.name].values():
            self.destroy_publisher(pub)
          
          del self.camera_publishers[request.name]

      except Exception as e:
        self.get_logger().error(f"Error al borrar la cámara '{request.name}': {str(e)}")
        response.success = False
    else:
      self.get_logger().warn(f"No se pudo borrar: La cámara '{request.name}' no existe.")
      response.success = False
      
    return response

def main(args=None):
  rclpy.init(args=args)
  node = PyBulletIKServer()
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