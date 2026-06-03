import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    pkg_mi_robot = get_package_share_directory('robot_pkg')
    urdf_file = os.path.join(pkg_mi_robot, 'urdf', 'robot.urdf')

    with open(urdf_file, 'r') as infp:
        robot_desc = infp.read()

    config = os.path.join(
        get_package_share_directory('robot_pkg'),
        'config',
        'robot_params.yaml'
    )

    return LaunchDescription([

        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_desc}]
        ),

        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen'
        ),

        Node(
            package='robot_kinematics',
            executable='robot_kinematics',
            name='robot_kinematics',
            output='screen'
        ),

        Node(
            package='robot_digital_twin',
            executable='robot_digital_twin_node',
            name='digital_twin',
            output='screen'
        ),


        Node(
            package='robot_pkg',
            executable='move_arm_node',
            name='move_arm_node',
            output='screen',
            parameters=[config]
        ),

        Node(
            package='robot_pkg',
            executable='rviz_bridge_node',
            name='rviz_bridge_node',
            output='screen',
            parameters=[config]
        ),

        Node(
            package='robot_pkg',
            executable='servo_bridge_node',
            name='servo_bridge_node',
            output='screen',
            parameters=[config]
        )
        
    ])