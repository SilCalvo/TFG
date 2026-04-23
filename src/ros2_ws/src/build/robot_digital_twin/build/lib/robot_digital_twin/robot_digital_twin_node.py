import os
import rclpy
from rclpy.node import Node
import pybullet as p
import math
import numpy as np
import json
from sensor_msgs.msg import Image, CameraInfo, JointState
from geometry_msgs.msg import TransformStamped
from tf2_ros import TransformBroadcaster
from robot_interfaces.srv import SpawnObject, AddCamera
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

        # 3. Suscripción al Motor Cinemático (Cerebro)
        # El cerebro debe publicar en /robot_joint_states tanto nombres como posiciones
        self.create_subscription(JointState, '/robot_joint_states', self.update_robot_callback, 10)

        # 4. Servicios de Escenario (Objetos y Cámaras)
        self.srv_spawn = self.create_service(SpawnObject, 'add_object', self.spawn_object_callback)
        self.srv_remove_obj = self.create_service(SpawnObject, 'remove_object', self.remove_object_callback)
        self.srv_camera = self.create_service(AddCamera, 'add_camera', self.add_camera_callback)
        self.srv_remove_cam = self.create_service(AddCamera, 'remove_camera', self.remove_camera_callback)

        # 5. Estado y Publicadores de Visión
        self.enviroment_objects = {}
        self.cameras_config = {}
        self.camera_publishers = {}
        self.camera_timer = None
        self.tf_broadcaster = TransformBroadcaster(self)

        self.get_logger().info("Gemelo Digital (GUI) iniciado y sincronizado por nombre de articulación.")

    # --- CALLBACK DE MOVIMIENTO ---
    def update_robot_callback(self, msg):
        """Mueve el robot visual buscando cada joint por su nombre en el mapa."""
        for name, position in zip(msg.name, msg.position):
            if name in self.joint_map:
                p.resetJointState(self.robot_id, self.joint_map[name], position)

    # --- GESTIÓN DE OBJETOS ---
    def spawn_object_callback(self, request, response):
        try:
            if request.name in self.enviroment_objects:
                p.removeBody(self.enviroment_objects[request.name])

            pkg_path = get_package_share_directory(request.package_name)
            urdf_name = f"{request.piece_type.lower()}.urdf"
            urdf_file_path = os.path.join(pkg_path, 'urdf', urdf_name)

            obj_id = p.loadURDF(
                urdf_file_path, 
                basePosition=[request.x, request.y, request.z],
                useFixedBase=True 
            )
            
            self.enviroment_objects[request.name] = obj_id
            self.get_logger().info(f"Objeto '{request.name}' añadido al Gemelo Digital.")
            response.success = True
        except Exception as e:
            self.get_logger().error(f"Error al spawnear objeto visual: {e}")
            response.success = False
        return response

    def remove_object_callback(self, request, response):
        if request.name in self.enviroment_objects:
            p.removeBody(self.enviroment_objects.pop(request.name))
            response.success = True
        else:
            response.success = False
        return response

    # --- GESTIÓN DE CÁMARAS ---
    def add_camera_callback(self, request, response):
        try:
            fov = 60.0
            near, far = 0.01, 10.0
            aspect = float(request.width) / float(request.height)

            view_matrix = p.computeViewMatrix(
                cameraEyePosition=[request.x, request.y, request.z],
                cameraTargetPosition=[request.target_x, request.target_y, request.target_z],
                cameraUpVector=[0, 0, 1]
            )
            proj_matrix = p.computeProjectionMatrixFOV(fov, aspect, near, far)

            # Intrínsecas para CameraInfo
            f_y = (request.height / 2.0) / math.tan(math.radians(fov) / 2.0)
            f_x = f_y
            c_x, c_y = request.width / 2.0, request.height / 2.0

            self.cameras_config[request.name] = {
                'view': view_matrix, 'proj': proj_matrix,
                'width': request.width, 'height': request.height,
                'near': near, 'far': far,
                'fx': f_x, 'fy': f_y, 'cx': c_x, 'cy': c_y
            }

            if request.name not in self.camera_publishers:
                self.camera_publishers[request.name] = {
                    'rgb': self.create_publisher(Image, f'/camera/{request.name}/color/image_raw', 10),
                    'depth': self.create_publisher(Image, f'/camera/{request.name}/depth/image_raw', 10),
                    'info': self.create_publisher(CameraInfo, f'/camera/{request.name}/camera_info', 10)
                }

            if self.camera_timer is None:
                self.camera_timer = self.create_timer(0.1, self.publish_camera_images)

            response.success = True
        except Exception as e:
            self.get_logger().error(f"Error al añadir cámara visual: {e}")
            response.success = False
        return response

    def publish_camera_images(self):
        """Renderiza y publica imágenes desde la ventana GUI."""
        for name, config in self.cameras_config.items():
            _, _, rgb, depth, _ = p.getCameraImage(
                width=config['width'], height=config['height'],
                viewMatrix=config['view'], projectionMatrix=config['proj'],
                renderer=p.ER_BULLET_HARDWARE_OPENGL
            )

            stamp = self.get_clock().now().to_msg()
            frame_id = f"camera_{name}_optical_frame"

            # Imagen Color
            msg_rgb = Image()
            msg_rgb.header.stamp, msg_rgb.header.frame_id = stamp, frame_id
            msg_rgb.height, msg_rgb.width = config['height'], config['width']
            msg_rgb.encoding, msg_rgb.step = "rgba8", config['width'] * 4
            msg_rgb.data = np.array(rgb, dtype=np.uint8).tobytes()
            self.camera_publishers[name]['rgb'].publish(msg_rgb)

            # Imagen Profundidad (Metros reales)
            depth_np = np.array(depth, dtype=np.float32)
            far, near = config['far'], config['near']
            depth_real = (far * near) / (far - (far - near) * depth_np)
            
            msg_depth = Image()
            msg_depth.header.stamp, msg_depth.header.frame_id = stamp, frame_id
            msg_depth.height, msg_depth.width = config['height'], config['width']
            msg_depth.encoding, msg_depth.step = "32FC1", config['width'] * 4
            msg_depth.data = depth_real.tobytes()
            self.camera_publishers[name]['depth'].publish(msg_depth)

            # Camera Info
            msg_info = CameraInfo()
            msg_info.header.stamp, msg_info.header.frame_id = stamp, frame_id
            msg_info.height, msg_info.width = config['height'], config['width']
            msg_info.k = [config['fx'], 0.0, config['cx'], 0.0, config['fy'], config['cy'], 0.0, 0.0, 1.0]
            msg_info.p = [config['fx'], 0.0, config['cx'], 0.0, 0.0, config['fy'], config['cy'], 0.0, 0.0, 0.0, 1.0, 0.0]
            self.camera_publishers[name]['info'].publish(msg_info)

    def remove_camera_callback(self, request, response):
        if request.name in self.cameras_config:
            del self.cameras_config[request.name]
            response.success = True
        else:
            response.success = False
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