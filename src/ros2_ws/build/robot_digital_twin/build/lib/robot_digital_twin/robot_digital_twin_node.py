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

    # --- CALLBACK DE SINCRONIZACIÓN DE ENTORNO (PAREDES) ---
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
                # Rojo semitransparente para identificar que es una pared virtual
                vis_id = p.createVisualShape(p.GEOM_BOX, halfExtents=half_extents, rgbaColor=[1, 0, 0, 0.4])
                
                wall_id = p.createMultiBody(
                    baseMass=0,
                    baseCollisionShapeIndex=col_id,
                    baseVisualShapeIndex=vis_id,
                    basePosition=[data['x'], data['y'], data['z']]
                )
                self.visual_walls[name] = wall_id
                self.get_logger().info(f"Pared visual sincronizada: {name}")

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
            fov, near, far = 60.0, 0.01, 10.0
            aspect = float(request.width) / float(request.height)

            view_matrix = p.computeViewMatrix(
                cameraEyePosition=[request.x, request.y, request.z],
                cameraTargetPosition=[request.target_x, request.target_y, request.target_z],
                cameraUpVector=[0, 0, 1]
            )
            proj_matrix = p.computeProjectionMatrixFOV(fov, aspect, near, far)

            # Intrínsecas
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
            self.get_logger().error(f"Error Add Camera: {e}")
            response.success = False
        return response

    def publish_camera_images(self):
        for name, config in self.cameras_config.items():
            _, _, rgb, depth, _ = p.getCameraImage(
                config['width'], config['height'],
                config['view'], config['proj'],
                renderer=p.ER_BULLET_HARDWARE_OPENGL
            )

            stamp = self.get_clock().now().to_msg()
            frame_id = f"camera_{name}_optical_frame"

            # Color
            msg_rgb = Image()
            msg_rgb.header.stamp, msg_rgb.header.frame_id = stamp, frame_id
            msg_rgb.height, msg_rgb.width = config['height'], config['width']
            msg_rgb.encoding, msg_rgb.step = "rgba8", config['width'] * 4
            msg_rgb.data = np.array(rgb, dtype=np.uint8).tobytes()
            self.camera_publishers[name]['rgb'].publish(msg_rgb)

            # Depth
            depth_np = np.array(depth, dtype=np.float32)
            far, near = config['far'], config['near']
            depth_real = (far * near) / (far - (far - near) * depth_np)
            msg_depth = Image()
            msg_depth.header.stamp, msg_depth.header.frame_id = stamp, frame_id
            msg_depth.height, msg_depth.width = config['height'], config['width']
            msg_depth.encoding, msg_depth.step = "32FC1", config['width'] * 4
            msg_depth.data = depth_real.tobytes()
            self.camera_publishers[name]['depth'].publish(msg_depth)

            # Info
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