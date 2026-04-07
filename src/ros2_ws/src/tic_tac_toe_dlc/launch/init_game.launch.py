from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([

        Node(
            package='tic_tac_toe_dlc',
            executable='tic_tac_toe_node',
            name='tic_tac_toe_node',
            output='screen',
            emulate_tty=True 
        )
        
    ])