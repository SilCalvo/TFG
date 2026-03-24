import os  
import rclpy
from rclpy.node import Node
import pybullet as p

from robot_interfaces.srv import SolveIK

class PyBulletIKServer(Node):
    def __init__(self):
        super().__init__('ik_server_node')
        self.srv = self.create_service(SolveIK, 'solve_ik', self.ik_callback)
        self.physicsClient = p.connect(p.DIRECT)
        
        urdf_path = os.path.expanduser("~/robot_ws/src/mi_robot_pkg/urdf/robot.urdf") 
        
        if not os.path.exists(urdf_path):
            self.get_logger().error(f"¡URDF no encontrado en la ruta: {urdf_path}!")
            self.get_logger().error("Revisa que el archivo exista exactamente ahí.")
        else:
            self.get_logger().info(f"Archivo URDF localizado en: {urdf_path}")
        
        self.robot_id = p.loadURDF(urdf_path, useFixedBase=True)
        self.end_effector_index = 5
        
        self.get_logger().info('Servidor IK en línea y esperando coordenadas...')

    def ik_callback(self, request, response):
        tx = request.target_pose.position.x
        ty = request.target_pose.position.y
        tz = request.target_pose.position.z
        
        try:
            joint_poses = p.calculateInverseKinematics(
                self.robot_id, self.end_effector_index, [tx, ty, tz]
            )
            response.joint_angles = list(joint_poses)
            response.success = True
            self.get_logger().info(f'Meta: x={tx:.2f}, y={ty:.2f}, z={tz:.2f} -> IK Calculada con éxito')
        except Exception as e:
            self.get_logger().error(f'Error al calcular IK: {str(e)}')
            response.joint_angles = []
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