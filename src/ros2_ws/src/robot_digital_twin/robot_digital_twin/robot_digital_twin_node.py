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
        
        self.physicsClient = p.connect(p.GUI)
        p.configureDebugVisualizer(p.COV_ENABLE_GUI, 1)
        p.setGravity(0, 0, -9.81)

        # Load Robot 
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
                self.get_logger().info(f"Mapped: '{joint_name}' -> ID {i}")
        except Exception as e:
            self.get_logger().error(f"Critical error loading robot: {e}")

        # SUBSCRIPTIONS
        # Robot movement (from C++ bridge)
        self.create_subscription(JointState, '/joint_states', self.update_robot_callback, 10)
        
        # Wall synchronization (from kinematics engine)
        self.create_subscription(String, '/env_sync', self.sync_callback, 10)

        # SERVICES (Scene and Cameras)
        self.srv_spawn = self.create_service(SpawnObject, 'add_object', self.spawn_object_callback)
        self.srv_remove_obj = self.create_service(SpawnObject, 'remove_object', self.remove_object_callback)
        self.srv_camera = self.create_service(AddCamera, 'add_camera', self.add_camera_callback)
        self.srv_remove_cam = self.create_service(AddCamera, 'remove_camera', self.remove_camera_callback)

        # State and Vision Publishers
        self.visual_walls = {}
        self.enviroment_objects = {}
        self.cameras_config = {}
        self.camera_publishers = {}
        self.camera_timer = None
        self.tf_broadcaster = TransformBroadcaster(self)

        self.get_logger().info("Digital Twin ready. Listening to /joint_states and /env_sync")

    # --- MOVEMENT CALLBACK ---
    def update_robot_callback(self, msg):
        """Updates the visual position by looking up joints by name."""
        for name, position in zip(msg.name, msg.position):
            if name in self.joint_map:
                p.resetJointState(self.robot_id, self.joint_map[name], position)

    # --- ENVIRONMENT SYNC CALLBACK (OBSTACLES) ---
    def sync_callback(self, msg):
        """Receives JSON from the kinematics engine to replicate walls."""
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
                self.get_logger().info(f"Visual wall synchronized with rotation: {name}")

            elif method == "remove_wall":
                if name in self.visual_walls:
                    p.removeBody(self.visual_walls[name])
                    del self.visual_walls[name]
                    self.get_logger().info(f"Visual wall '{name}' removed.")
                else:
                    self.get_logger().warn(f"Attempt to remove non-existent wall: {name}")

        except Exception as e:
            self.get_logger().error(f"Error in sync_callback: {e}")

    # --- OBJECT MANAGEMENT (PIECES) ---
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

    # --- RGB-D CAMERA SYSTEM ---
    
    def add_camera_callback(self, request, response):
        try:
            fov = 60.0
            near = 0.01
            far = 10.0

            # Define position and target
            pos = np.array([request.x, request.y, request.z])
            target = np.array([request.target_x, request.target_y, request.target_z])
            
            # Calculate base vectors (without rotation)
            forward = target - pos
            forward = forward / np.linalg.norm(forward)
            
            world_up = np.array([0.0, 0.0, 1.0])
            # Singularity prevention: if the camera looks exactly up/down
            if abs(np.dot(forward, world_up)) > 0.999:
                world_up = np.array([1.0, 0.0, 0.0])
                
            right_base = np.cross(forward, world_up)
            right_base = right_base / np.linalg.norm(right_base)
            up_base = np.cross(right_base, forward)

            # Apply 90-degree rotation on Z axis (Roll / Optical axis)
            roll_angle = math.radians(90.0) 
            
            # Calculate the new vectors after roll rotation
            camera_up = up_base * math.cos(roll_angle) + right_base * math.sin(roll_angle)
            camera_up = camera_up / np.linalg.norm(camera_up)
            new_right = np.cross(forward, camera_up)
            new_right = new_right / np.linalg.norm(new_right)
            new_down = np.cross(forward, new_right)

            # Generate PyBullet matrices with the rotated camera
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

            # Intrinsic parameters (Pinhole Model)
            fov_rad = math.radians(fov)
            f_y = (request.height / 2.0) / math.tan(fov_rad / 2.0)
            f_x = f_y  # Assuming square pixels
            c_x = request.width / 2.0
            c_y = request.height / 2.0

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

            # Save general configuration and Publishers
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
                self.get_logger().info("First camera detected. Activating RGB-D vision system (10Hz)...")
                # Start the timer to publish camera images at 10Hz
                self.camera_timer = self.create_timer(0.1, self.publish_camera_images)

            self.get_logger().info(f"RGB-D camera '{request.name}' configured with 90 degree rotation.")
            response.success = True

        except Exception as e:
            self.get_logger().error(f"Error adding camera: {str(e)}")
            response.success = False
            
        return response
    
    def publish_camera_images(self):
        if not self.cameras_config:
            self.get_logger().info("No cameras found. Shutting down vision system.")
            self.destroy_timer(self.camera_timer)
            self.camera_timer = None
            return

        for name, config in self.cameras_config.items():
            try:
                # Full image capture (Color + Depth)
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
                msg_depth.encoding = "32FC1" 
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
                msg_info.d = [0.0, 0.0, 0.0, 0.0, 0.0] # Perfect lens with no distortion
                
                # K = [fx, 0, cx, 0, fy, cy, 0, 0, 1]
                msg_info.k = [config['fx'], 0.0, config['cx'], 
                            0.0, config['fy'], config['cy'], 
                            0.0, 0.0, 1.0]
                
                # P = Rectified projection matrix
                msg_info.p = [config['fx'], 0.0, config['cx'], 0.0,
                            0.0, config['fy'], config['cy'], 0.0,
                            0.0, 0.0, 1.0, 0.0]
                
                self.camera_publishers[name]['info'].publish(msg_info)
                
                # --- LINK FRAME PUBLICATION (Z up, X forward) ---
                t_link = TransformStamped()
                t_link.header.stamp = stamp
                t_link.header.frame_id = 'base' # Change to 'base_link' if preferred
                t_link.child_frame_id = frame_id
                
                t_link.transform.translation.x = config['pos'][0]
                t_link.transform.translation.y = config['pos'][1]
                t_link.transform.translation.z = config['pos'][2]
                t_link.transform.rotation.x = config['quat'][0]
                t_link.transform.rotation.y = config['quat'][1]
                t_link.transform.rotation.z = config['quat'][2]
                t_link.transform.rotation.w = config['quat'][3]
                
                # --- OPTICAL FRAME PUBLICATION (Z forward, Y down) ---
                t_opt = TransformStamped()
                t_opt.header.stamp = stamp
                t_opt.header.frame_id = frame_id 
                t_opt.child_frame_id = f"camera_{name}_optical_frame"
                
                t_opt.transform.rotation.x = 0.0
                t_opt.transform.rotation.y = 0.7071
                t_opt.transform.rotation.z = 0.0
                t_opt.transform.rotation.w = 0.7071

                self.tf_broadcaster.sendTransform([t_link, t_opt])

            except Exception as e:
                self.get_logger().error(f"Error in RGB-D streaming of {name}: {str(e)}")

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