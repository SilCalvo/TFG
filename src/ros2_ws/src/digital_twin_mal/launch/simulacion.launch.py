import os
import launch
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from webots_ros2_driver.webots_launcher import WebotsLauncher
from webots_ros2_driver.utils import controller_url_prefix

def generate_launch_description():
    pkg_robot = get_package_share_directory('robot_pkg')
    pkg_twin = get_package_share_directory('digital_twin')
    
    urdf_file = os.path.join(pkg_robot, 'urdf', 'robot.urdf')
    world_file = os.path.join(pkg_twin, 'worlds', 'empty.wbt')

    # Lanzador de Webots
    # Añadimos ros_distro para que no se líe con las rutas en WSL
    webots = WebotsLauncher(
        world=world_file,
        mode='realtime',
        stream=False
    )

    # Driver de Webots
    webots_driver = Node(
        package='webots_ros2_driver',
        executable='driver',
        output='screen',
        additional_env={'WEBOTS_CONTROLLER_URL': controller_url_prefix() + 'robot_cilindros'},
        parameters=[
            {'robot_description': urdf_file},
            {'use_sim_time': True},
            {'set_robot_state_publisher': True}
        ]
    )

    # Tu nodo del Gemelo Digital
    digital_twin_node = Node(
        package='digital_twin',
        executable='digital_twin',
        name='digital_twin_node',
        output='screen',
        parameters=[{'use_sim_time': True}]
    )

    return LaunchDescription([
        #webots,
        webots_driver,
        digital_twin_node,
        # Esto cierra ROS si cierras Webots
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=webots,
                on_exit=[launch.actions.EmitEvent(event=launch.events.Shutdown())],
            )
        )
    ])