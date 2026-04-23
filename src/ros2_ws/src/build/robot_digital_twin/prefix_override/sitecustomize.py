import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/silvia/TFG/TFG/src/ros2_ws/src/install/robot_digital_twin'
