import os
import rclpy
from rclpy.node import Node
import pybullet as p
import math
import numpy as np
import json
from sensor_msgs.msg import Image, CameraInfo, JointState
from std_msgs.msg import String
from geometry_msgs.msg import TransformStamped
from tf2_ros import TransformBroadcaster
from robot_interfaces.srv import SpawnObject, AddCamera, RemoveObstacle
from ament_index_python.packages import get_package_share_directory

class DigitalTwinVisualizer(Node):
    def __init__(self):
        super().__init__('digital_twin_visualizer')
        
        # 1. Inicialización de PyBullet con GUI
        self.physicsClient = p.connect(p.GUI)
        p.configureDebugVisualizer(p.COV_ENABLE_GUI, 1)
        p.setGravity(0, 0, -9.81)

        # 2. Cargar Robot y Crear Mapa de Articulaciones (Nombre -> Índice)
        self.joint_map = {}
        try:
            # Asegúrate de que 'robot_pkg' es el nombre correcto de tu paquete
            pkg_path = get_package_share_directory('robot_pkg') 
            urdf_path = os.path.join(pkg_path, 'urdf', 'robot.urdf')
            self.robot_id = p.loadURDF(urdf_path, useFixedBase=True)
            
            num_joints = p.getNumJoints(self.robot_id)
            for i in range(num_joints):
                info = p.getJointInfo(self.robot_id, i)
                joint_name = info[1].decode('utf-8')
                self.joint_map[joint_name] = i
                self.get_logger().info(f"Mapeado: '{joint_name}' -> ID {i}")
        except Exception as e:
            self.get_logger().error(f"Error crítico cargando robot: {e}")

        # 3. SUSCRIPCIONES (Escuchas)
        # Movimiento del robot (desde el puente C++)
        self.create_subscription(JointState, '/joint_states', self.update_robot_callback, 10)
        
        # Sincronización de paredes (desde el motor cinemático)
        self.create_subscription(String, '/env_sync', self.sync_callback, 10)

        # 4. SERVICIOS (Escenario y Cámaras)
        self.srv_spawn = self.create_service(SpawnObject, 'add_object', self.spawn_object_callback)
        self.srv_remove_obj = self.create_service(SpawnObject, 'remove_object', self.remove_object_callback)
        self.srv_camera = self.create_service(AddCamera, 'add_camera', self.add_camera_callback)
        self.srv_remove_cam = self.create_service(AddCamera, 'remove_camera', self.remove_camera_callback)

        # 5. Estado y Publicadores de Visión
        self.visual_walls = {}
        self.enviroment_objects = {}
        self.cameras_config = {}
        self.camera_publishers = {}
        self.camera_timer = None
        self.tf_broadcaster = TransformBroadcaster(self)

        self.get_logger().info("Gemelo Digital listo. Escuchando /joint_states y /env_sync")

    # --- CALLBACK DE MOVIMIENTO ---
    def update_robot_callback(self, msg):
        """Actualiza la posición visual buscando joints por nombre."""
        for name, position in zip(msg.name, msg.position):
            if name in self.joint_map:
                p.resetJointState(self.robot_id, self.joint_map[name], position)

    # --- CALLBACK DE SINCRONIZACIÓN DE ENTORNO (OBSTACULOS) ---
    def sync_callback(self, msg):
        """Recibe JSON del motor cinemático para replicar paredes."""
        try:
            data = json.loads(msg.data)
            method = data.get("method")
            name = data.get("name")

            if method == "add_wall":
                if name in self.visual_walls:
                    p.removeBody(self.visual_walls[name])

                half_extents = [data['width']/2, data['depth']/2, data['height']/2]
                col_id = p.createCollisionShape(p.GEOM_BOX, halfExtents=half_extents)
                vis_id = p.createVisualShape(p.GEOM_BOX, halfExtents=half_extents, rgbaColor=[1, 0, 0, 0.4])
                
                roll = data.get('roll', 0.0)
                pitch = data.get('pitch', 0.0)
                yaw = data.get('yaw', 0.0)
                
                orientation_q = p.getQuaternionFromEuler([roll, pitch, yaw])
                
                wall_id = p.createMultiBody(
                    baseMass=0,
                    baseCollisionShapeIndex=col_id,
                    baseVisualShapeIndex=vis_id,
                    basePosition=[data['x'], data['y'], data['z']],
                    baseOrientation=orientation_q
                )
                self.visual_walls[name] = wall_id
                self.get_logger().info(f"Pared visual sincronizada con rotación: {name}")

            elif method == "remove_wall":
                if name in self.visual_walls:
                    p.removeBody(self.visual_walls[name])
                    del self.visual_walls[name]
                    self.get_logger().info(f"Pared visual '{name}' eliminada.")
                else:
                    self.get_logger().warn(f"Intento de borrar pared inexistente: {name}")

        except Exception as e:
            self.get_logger().error(f"Error en sync_callback: {e}")

    # --- GESTIÓN DE OBJETOS (PIEZAS) ---
    def spawn_object_callback(self, request, response):
        try:
            if request.name in self.enviroment_objects:
                p.removeBody(self.enviroment_objects[request.name])

            pkg_path = get_package_share_directory(request.package_name)
            urdf_file = os.path.join(pkg_path, 'urdf', f"{request.piece_type.lower()}.urdf")

            obj_id = p.loadURDF(urdf_file, [request.x, request.y, request.z], useFixedBase=True)
            self.enviroment_objects[request.name] = obj_id
            response.success = True
        except Exception as e:
            self.get_logger().error(f"Error Spawn Object: {e}")
            response.success = False
        return response

    def remove_object_callback(self, request, response):
        if request.name in self.enviroment_objects:
            p.removeBody(self.enviroment_objects.pop(request.name))
            response.success = True
        else:
            response.success = False
        return response

    # --- SISTEMA DE CÁMARAS RGB-D ---
    
    def add_camera_callback(self, request, response):
        try:
            fov = 60.0
            near = 0.01
            far = 10.0

            # 1. Definir posición y objetivo
            pos = np.array([request.x, request.y, request.z])
            target = np.array([request.target_x, request.target_y, request.target_z])
            
            # 2. Calcular los vectores base originales (sin rotación)
            forward = target - pos
            forward = forward / np.linalg.norm(forward)
            
            world_up = np.array([0.0, 0.0, 1.0])
            # Prevención de singularidad: si la cámara mira exactamente hacia abajo/arriba
            if abs(np.dot(forward, world_up)) > 0.999:
                world_up = np.array([1.0, 0.0, 0.0])
                
            right_base = np.cross(forward, world_up)
            right_base = right_base / np.linalg.norm(right_base)
            up_base = np.cross(right_base, forward)

            # 3. Aplicar rotación de 90 grados en el eje Z (Roll / Eje óptico)
            roll_angle = math.radians(90.0) # Cambia a -90.0 si el giro es en la dirección opuesta
            
            # Calculamos el nuevo vector Up aplicando la Fórmula de Rodrigues
            camera_up = up_base * math.cos(roll_angle) + right_base * math.sin(roll_angle)
            camera_up = camera_up / np.linalg.norm(camera_up)

            # Recalculamos ejes Right y Down tras el giro para el TF de ROS
            new_right = np.cross(forward, camera_up)
            new_right = new_right / np.linalg.norm(new_right)
            new_down = np.cross(forward, new_right)

            # 4. Generar matrices de PyBullet con la cámara girada
            view_matrix = p.computeViewMatrix(
                cameraEyePosition=pos.tolist(),
                cameraTargetPosition=target.tolist(),
                cameraUpVector=camera_up.tolist()
            )
            
            proj_matrix = p.computeProjectionMatrixFOV(
                fov=fov, 
                aspect=float(request.width)/float(request.height), 
                nearVal=near, 
                farVal=far
            )

            # 5. CÁLCULO DE INTRÍNSECAS (Modelo Pinhole)
            fov_rad = math.radians(fov)
            f_y = (request.height / 2.0) / math.tan(fov_rad / 2.0)
            f_x = f_y  # Asumimos píxeles cuadrados
            c_x = request.width / 2.0
            c_y = request.height / 2.0

            # 6. Cálculo del cuaternión para la transformada (TF) usando los ejes rotados
            rot_matrix = np.array([
                [new_right[0], new_down[0], forward[0]],
                [new_right[1], new_down[1], forward[1]],
                [new_right[2], new_down[2], forward[2]]
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

            # 7. Guardar configuración general y Publishers
            self.cameras_config[request.name] = {
                'view': view_matrix,
                'proj': proj_matrix,
                'width': request.width,
                'height': request.height,
                'near': near,
                'far': far,
                'fx': f_x, 'fy': f_y, 'cx': c_x, 'cy': c_y,
                'pos': pos.tolist(),
                'quat': [qx, qy, qz, qw]
            }

            if request.name not in self.camera_publishers:
                self.camera_publishers[request.name] = {
                    'rgb': self.create_publisher(Image, f'/camera/{request.name}/color/image_raw', 10),
                    'depth': self.create_publisher(Image, f'/camera/{request.name}/depth/image_raw', 10),
                    'info': self.create_publisher(CameraInfo, f'/camera/{request.name}/camera_info', 10)
                }

            if self.camera_timer is None:
                self.get_logger().info("Detectada primera cámara. Activando sistema de visión RGB-D (10Hz)...")
                self.camera_timer = self.create_timer(0.1, self.publish_camera_images)

            self.get_logger().info(f"Cámara RGB-D '{request.name}' configurada con rotación de 90 grados.")
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

    def remove_camera_callback(self, request, response):
        if request.name in self.cameras_config:
            del self.cameras_config[request.name]
            response.success = True
        return response

def main(args=None):
    rclpy.init(args=args)
    node = DigitalTwinVisualizer()
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