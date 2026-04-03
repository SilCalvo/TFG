import os  
import rclpy
from rclpy.node import Node
import pybullet as p
import math
import time

from robot_interfaces.srv import SolveIK, AddObstacle, RemoveObstacle

class PyBulletIKServer(Node):
    def __init__(self):
        super().__init__('ik_server_node')
        # Inicialización de servicios
        self.srv_ik = self.create_service(SolveIK, 'solve_ik', self.ik_callback)
        self.srv_add = self.create_service(AddObstacle, 'add_wall', self.add_wall_callback)
        self.srv_rem = self.create_service(RemoveObstacle, 'remove_wall', self.remove_wall_callback)
        
        self.walls = {}
        self.physicsClient = p.connect(p.GUI)
        
        # Configuración del Robot
        urdf_path = os.path.expanduser("~/robot_ws/src/mi_robot_pkg/urdf/robot.urdf") 
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
                
        # Texto Debug Inicial
        initial_pos = p.getLinkState(self.robot_id, self.end_effector_index)[4]
        self.debug_text_id = p.addUserDebugText(
            text=f"X: {initial_pos[0]:.2f} Y: {initial_pos[1]:.2f} Z: {initial_pos[2]:.2f}",
            textPosition=[initial_pos[0], initial_pos[1], initial_pos[2] + 0.1],
            textColorRGB=[1, 0, 0], textSize=1.5
        )
        
        self.get_logger().info('Servidor IK y Obstáculos listo.')

    def ik_callback(self, request, response):
        tx, ty, tz = request.target_pose.position.x, request.target_pose.position.y, request.target_pose.position.z
        
        try:
            # 1. GUARDAR ESTADO ACTUAL
            saved_joint_states = [p.getJointState(self.robot_id, j)[0] for j in self.movable_joints]

            # 2. CALCULAR IK (Nota: calculateInverseKinematics devuelve una tupla con TODOS los joints, incluyendo fijos)
            # PyBullet suele devolver valores para todos los joints, filtramos o asignamos con cuidado.
            all_joint_poses = p.calculateInverseKinematics(self.robot_id, self.end_effector_index, [tx, ty, tz])
            
            # 3. TELETRANSPORTAR Y VALIDAR
            for i, joint_idx in enumerate(self.movable_joints):
                p.resetJointState(self.robot_id, joint_idx, all_joint_poses[i])                    
            
            actual_pos = p.getLinkState(self.robot_id, self.end_effector_index)[4]
            
            # 4. COMPROBAR COLISIONES
            colision = False
            wall_name_colision = ''
            p.performCollisionDetection() # Forzar a PyBullet a calcular contactos
            
            for name, wall_id in self.walls.items():
                contacts = p.getContactPoints(bodyA=self.robot_id, bodyB=wall_id)
                if len(contacts) > 0:
                    colision = True
                    wall_name_colision = name  
                    break

            # 5. VALIDACIÓN FINAL
            error_dist = math.sqrt(sum((a - b)**2 for a, b in zip(actual_pos, [tx, ty, tz])))
            tolerancia = 0.05
            
            if error_dist > tolerancia or colision:
                if colision:
                    self.get_logger().warn(f'¡COLISIÓN detectada con pared: {wall_name_colision}!')
                else:
                    self.get_logger().warn(f'Fuera de alcance (Error: {error_dist:.3f}m)')
                time.sleep(2)
                
                # REBOBINAR
                for i, joint_idx in enumerate(self.movable_joints):
                    p.resetJointState(self.robot_id, joint_idx, saved_joint_states[i])
                
                response.success = False
                response.joint_angles = []
            else:
                self.get_logger().info('IK Exitosa. (Error: {error_dist:.3f}m)')
                response.joint_angles = [all_joint_poses[i] for i in range(len(self.movable_joints))]
                response.success = True

            # Actualizar Texto Visual
            self.update_debug_text(p.getLinkState(self.robot_id, self.end_effector_index)[4])

        except Exception as e:
            self.get_logger().error(f'Error IK: {str(e)}')
            response.success = False
            
        return response

    # --- MÉTODOS DE GESTIÓN DE OBSTÁCULOS  ---

    def add_wall_callback(self, request, response):
        try:
            # Si ya existe una pared con ese nombre, la borramos primero para no duplicar
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