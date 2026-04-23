import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # --- LECTURA DEL URDF ---
    pkg_mi_robot = get_package_share_directory('robot_pkg')
    urdf_file = os.path.join(pkg_mi_robot, 'urdf', 'robot.urdf')

    with open(urdf_file, 'r') as infp:
        robot_desc = infp.read()

    return LaunchDescription([
        
        # 1. ESTADO DEL ROBOT 
        # Lee el URDF y publica la estructura estática del robot
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_desc}]
        ),

        # 2. RVIZ2 
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen'
        ),

        # 3. NODO DE PYTHON (PyBullet IK Server)
        # Se encarga de recibir coordenadas (X, Y, Z) y devolver los ángulos.
        Node(
            package='robot_kinematics',
            executable='robot_kinematics',
            name='robot_kinematics',
            output='screen'
        ),
        # 3. NODO DE PYTHON (PyBullet IK Server)
        # Se encarga de recibir coordenadas (X, Y, Z) y devolver los ángulos.
        Node(
            package='robot_digital_twin',
            executable='robot_digital_twin_node',
            name='digital_twin',
            output='screen'
        ),


        # 4. NODO DE C++ (Action Server / Interpolador)
        # Recibe la meta de Navegación, pide la IK a Python y publica grados suaves.
        Node(
            package='robot_pkg',
            executable='move_arm_node',
            name='move_action_server',
            output='screen'
        ),

        # 5. NODO PUENTE 
        # Lee los grados de 'robot_cmd', los pasa a radianes y los publica en 'joint_states' para RViz.
        Node(
            package='robot_pkg',
            executable='rviz_bridge_node',
            name='rviz_bridge',
            output='screen'
        )
        
    ])