import os  
import rclpy
from rclpy.node import Node
import pybullet as p
import math
import time

from robot_interfaces.srv import SolveIK, AddObstacle, RemoveObstacle, SolveDK
from ament_index_python.packages import get_package_share_directory

class PyBulletIKServer(Node):
    def __init__(self):
        super().__init__('ik_server_node')
        # Inicialización de servicios
        self.srv_ik = self.create_service(SolveIK, 'solve_ik', self.ik_callback)
        self.srv_dk = self.create_service(SolveDK, 'solve_dk', self.dk_callback)
        self.srv_add = self.create_service(AddObstacle, 'add_wall', self.add_wall_callback)
        self.srv_rem = self.create_service(RemoveObstacle, 'remove_wall', self.remove_wall_callback)
        
        self.walls = {}
        self.physicsClient = p.connect(p.GUI)
        
        # Configuración del Robot
        pkg_path = get_package_share_directory('robot_pkg') # Pon el nombre de tu paquete aquí
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
                
        # Texto Debug Inicial
        initial_pos = p.getLinkState(self.robot_id, self.end_effector_index)[4]
        self.debug_text_id = p.addUserDebugText(
            text=f"X: {initial_pos[0]:.2f} Y: {initial_pos[1]:.2f} Z: {initial_pos[2]:.2f}",
            textPosition=[initial_pos[0], initial_pos[1], initial_pos[2] + 0.1],
            textColorRGB=[1, 0, 0], textSize=1.5
        )
        
        self.get_logger().info('Servidor IK y Obstáculos listo.')

    def ik_callback(self, request, response):
        # 1. Extraer objetivo (Punta de la herramienta)
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
            
            # --- 1. LÓGICA DE CÁLCULO TCP PRIMERO ---
            t_pos, t_orn = self.get_tcp_info(request.tcp_offset, request.tool_dimensions)

            angles_matered=True
            if all(v == 0 for v in target_orn):
                angles_matered=False
                _, curr_wrist_orn = p.getLinkState(self.robot_id, self.end_effector_index)[4:6]
                _, curr_tip_orn = p.multiplyTransforms([0,0,0], curr_wrist_orn, [0,0,0], t_orn)
                
                target_orn = curr_tip_orn

            # --- LÓGICA DE CÁLCULO TCP (CORREGIDA) ---
            
            # Invertimos la transformación COMPLETA (pos y orn)
            inv_pos, inv_orn = p.invertTransform(t_pos, t_orn)
            
            # Calculamos Muñeca = Target_Punta * TCP_Invertido
            wrist_target_pos, wrist_target_orn = p.multiplyTransforms(
                target_pos, target_orn, inv_pos, inv_orn
            )
            self.get_logger().info( f'Muneca destino: X={wrist_target_pos[0]:.3f}, Y={wrist_target_pos[1]:.3f}, Z={wrist_target_pos[2]:.3f}')
            self.get_logger().info( f'Muneca destino orinetacion: X={wrist_target_orn[0]:.3f}, Y={wrist_target_orn[1]:.3f}, Z={wrist_target_orn[2]:.3f}')

            # --- CÁLCULO IK ---
            if (not angles_matered):
                self.get_logger().info(f'No importa orientacion')
                joint_poses = p.calculateInverseKinematics(self.robot_id, self.end_effector_index, wrist_target_pos)
            else:
                self.get_logger().info(f'Si importa orientacion')
                joint_poses = p.calculateInverseKinematics(self.robot_id, self.end_effector_index, wrist_target_pos, wrist_target_orn)
            
            # VALIDACIÓN DE SEGURIDAD: ¿Hay algún NaN?
            if any(math.isnan(ang) for ang in joint_poses):
                self.get_logger().error("IK devolvió NaN. Objetivo inalcanzable.")
                response.success = False
                return response
            
            # --- VALIDACIÓN (El Teletransporte Doble) ---
            
            # 1. Teletransportamos el brazo del robot
            for i, joint_idx in enumerate(self.movable_joints):
                p.resetJointState(self.robot_id, joint_idx, joint_poses[i])

            # 2. Teletransportamos la herramienta para que no se quede atrás flotando
            new_wrist_state = p.getLinkState(self.robot_id, self.end_effector_index)
            new_tool_pos, new_tool_orn = p.multiplyTransforms(
                new_wrist_state[4], new_wrist_state[5], 
                self.tool_local_pos, 
                self.tool_local_orn
            )
            p.resetBasePositionAndOrientation(self.tool_body_id, new_tool_pos, new_tool_orn)

            # 3. Avanzamos 1 frame la física para asentar piezas
            p.stepSimulation()

            wrist_state = p.getLinkState(self.robot_id, self.end_effector_index)
            # USAR t_orn AQUÍ TAMBIÉN para que la punta sea real
            actual_tip_pos, _ = p.multiplyTransforms(wrist_state[4], wrist_state[5], t_pos, t_orn)
            w_pos = wrist_state[4] 

            self.get_logger().info(
                f'Muneca real: X={w_pos[0]:.3f}, Y={w_pos[1]:.3f}, Z={w_pos[2]:.3f} | '
                f'Punta real: X={actual_tip_pos[0]:.3f}, Y={actual_tip_pos[1]:.3f}, Z={actual_tip_pos[2]:.3f}'
            )
            
            # 7. COMPROBAR COLISIONES (Robot + Herramienta)
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

            # 8. VALIDACIÓN FINAL (Usando la punta real vs el objetivo)
            error_dist = math.sqrt(sum((a - b)**2 for a, b in zip(actual_tip_pos, target_pos)))
            tolerancia = 0.5 ############
            
            if error_dist > tolerancia or colision:
                if colision:
                    self.get_logger().warn(f'¡COLISIÓN detectada con pared: {wall_name_colision}!')
                else:
                    self.get_logger().warn(f'Meta inalcanzable (Error: {error_dist:.3f}m)')
                
                time.sleep(10) #############################################################################################################################
                # REBOBINAR
                for i, joint_idx in enumerate(self.movable_joints):
                    p.resetJointState(self.robot_id, joint_idx, saved_joint_states[i])
                
                response.success = False
                response.joint_angles = []
            else:
                self.get_logger().info(f'IK Exitosa. Error punta: {error_dist:.4f}m')
                response.joint_angles = [joint_poses[i] for i in range(len(self.movable_joints))]
                response.success = True

            # Actualizar Texto Visual en la punta
            self.update_debug_text(actual_tip_pos)

        except Exception as e:
            self.get_logger().error(f'Error crítico en IK: {str(e)}')
            response.success = False

        p.addUserDebugLine(target_pos, [target_pos[0]+0.1, target_pos[1], target_pos[2]], [1,0,0], 5) # Línea roja de 10cm
        return response
    
    def dk_callback(self, request, response):
        try:
            angles = request.joint_angles
            # 1. Guardar estado actual
            old_angles = [p.getJointState(self.robot_id, i)[0] for i in self.movable_joints]

            # 2. Teletransportar temporalmente
            for i, joint_idx in enumerate(self.movable_joints):
                p.resetJointState(self.robot_id, joint_idx, angles[i])

            w_pos, w_orn = p.getLinkState(self.robot_id, self.end_effector_index)[4:6]
    
            # USAMOS LA FUNCIÓN
            t_pos, t_orn = self.get_tcp_info(request.tcp_offset, request.tool_dimensions)
            
            # Calculamos Punta = Muñeca * TCP
            f_pos, f_orn = p.multiplyTransforms(w_pos, w_orn, t_pos, t_orn)

            # 5. REBOBINAR
            for i, joint_idx in enumerate(self.movable_joints):
                p.resetJointState(self.robot_id, joint_idx, old_angles[i])

            # Preparar respuesta
            response.target_pose.position.x, response.target_pose.position.y, response.target_pose.position.z = f_pos
            response.target_pose.orientation.x, response.target_pose.orientation.y, response.target_pose.orientation.z, response.target_pose.orientation.w = f_orn
            response.success = True

        except Exception as e:
            self.get_logger().error(f"Error en FK: {str(e)}")
            response.success = False
        
        return response

    def get_tcp_info(self, tcp_offset, tool_dimensions):
        """Calcula la pose de la punta combinando el offset de la base y el largo."""
        largo = tool_dimensions[0]
        
        # 1. Transformación de la Muñeca a la BASE de la herramienta
        base_pos = [tcp_offset.position.x, tcp_offset.position.y, tcp_offset.position.z]
        base_orn = [tcp_offset.orientation.x, tcp_offset.orientation.y, 
                    tcp_offset.orientation.z, tcp_offset.orientation.w]
        if all(v == 0 for v in base_orn): base_orn = [0, 0, 0, 1]

        # 2. Transformación de la Base al EXTREMO (Punta)
        # La punta siempre está en el Z local de la herramienta
        tip_local_pos = [0, 0, largo]
        tip_local_orn = [0, 0, 0, 1]

        # 3. Combinamos ambas para obtener la PUNTA respecto a la MUÑECA
        # Esto soluciona tu duda: si la base está rotada en Y, la punta saldrá inclinada
        t_pos, t_orn = p.multiplyTransforms(base_pos, base_orn, tip_local_pos, tip_local_orn)
        norm = math.sqrt(sum([v**2 for v in t_orn]))
        if norm > 0:
            t_orn = [v/norm for v in t_orn]
        
        return t_pos, t_orn
    

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

    def update_tool_collision(self, request):
        tcp = request.tcp_offset
        dims = request.tool_dimensions # Lista de floats: [largo, ancho, profundo] o [largo, radio]
        t_type = request.tool_type

        # 1. Validación de tipo (Solo 0: Caja o 1: Cilindro)
        if t_type not in [0, 1]:
            self.get_logger().error(f"Tipo de herramienta inválido: {t_type}. Solo se permite 0 (Caja) o 1 (Cilindro).")
            return False # Retornamos False para que el ik_callback sepa que falló

        # 2. Validación de dimensiones (Evitar que la lista esté vacía)
        if len(dims) < 2:
            self.get_logger().error("Dimensiones de herramienta insuficientes.")
            return False

        # 3. Eliminar herramienta previa si existe
        if hasattr(self, 'tool_body_id'):
            p.removeBody(self.tool_body_id)

        try:
            # Selección de forma geométrica
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

           # 1. Leer dónde está la muñeca AHORA MISMO
            wrist_state = p.getLinkState(self.robot_id, self.end_effector_index)
            w_pos = wrist_state[4]
            w_orn = wrist_state[5]# Calculamos la posición exacta sumando el tcp, el offset del centro y los 0.04 de la mano
            spawn_pos, spawn_orn = p.multiplyTransforms(
                w_pos, w_orn, 
                [tcp.position.x, tcp.position.y, tcp.position.z + z_offset + 0.04], 
                [tcp.orientation.x, tcp.orientation.y, tcp.orientation.z, tcp.orientation.w]
            )
            # GUARDAMOS EN SELF para poder usarlo luego en el teletransporte
            self.tool_local_pos = [tcp.position.x, tcp.position.y, tcp.position.z + z_offset + 0.04]
            self.tool_local_orn = [tcp.orientation.x, tcp.orientation.y, tcp.orientation.z, tcp.orientation.w]

            # --- 2. CREAR EL CUERPO ---
            self.tool_body_id = p.createMultiBody(
                baseMass=0.1, 
                baseCollisionShapeIndex=col_id, 
                baseVisualShapeIndex=vis_id,
                basePosition=spawn_pos,       # Nace en el sitio perfecto
                baseOrientation=spawn_orn     # Con la rotación perfecta
            )

            self.get_logger().info(f"Nuevo objeto herramienta creado con ID: {self.tool_body_id}")

            # --- 3. DESACTIVAR EL FUEGO AMIGO (Auto-colisión) ---
            # ¡ESTO FALTABA! Evita que la herramienta pelee físicamente contra la mano
            for j in range(-1, p.getNumJoints(self.robot_id)):
                p.setCollisionFilterPair(self.robot_id, self.tool_body_id, j, -1, enableCollision=0)

            # 4. Atar la herramienta al robot
            # Usamos p.multiplyTransforms para que el z_offset respete la orientación del TCP
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