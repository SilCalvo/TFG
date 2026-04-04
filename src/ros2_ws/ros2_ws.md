## Ros2 Jazzy + Pybullet

- [`src/`](./src/)  
  Carpeta con los paquetes de ros2.

    - [`robot_interfaces/`](./src/robot_interfaces/)  
      Paquete con las definiciones de los mensajes, servicios y acciones personalizados de ROS 2 para la comunicación entre nodos.

    - [`robot_imverse_kinematics/`](./src/robot_inverse_kinematics/)  
      Paquete con servidor cinemático que actúa como gemelo digital en PyBullet para resolver trayectorias, compensar el TCP y garantizar la evasión de colisiones.
  
    - [`robot_pkg/`](./src/robot_pkg/)  
      Paquete central del sistema que contiene la descripción del robot (URDF), los nodos de control principal, el ejecutor de rutinas y los archivos de despliegue (launch).


        - [`launch/`](./src/robot_pkg/launch/)  
          Obtiene los launchs disponibles.
          [`move_action/`](./src/robot_pkg/launch/move_action.launch.py) : Solo simulación.
          **FALTA AÑADIR EL CON FISICO**

        - [`rutines/`](./src/robot_pkg/rutines/)  
          Se encuentran los archivos de rutinas a ejecutar por el usuario.
          
          [`test_moveL/`](./src/robot_pkg/robot_pkg/rutines/test_moveL.yaml) : Rutina de testear MOVEL (por defecto).

          [`test_tcp/`](./src/robot_pkg/robot_pkg/rutines/test_tcp.yaml) : Rutina de testear el cambio de herramienta.

          [`test_wall/`](./src/robot_pkg/robot_pkg/rutines/test_wall.yaml) : Rutina de testear creacion de una pared virtual. Debe dar fallo porque no puede atravesar dicha pared virtual

        - [`src/`](./src/robot_pkg/src/)  
          Se encuentran los nodos del programa.
          
          [`move_arm/`](./src/robot_pkg/src/move_arm_node.cpp) : Nodo central del sistema. Es el que genera las trayectorias y las ejecuta.

          [`routine_executor/`](./src/robot_pkg/src/routine_executor.cpp) : Nodo que convierte las rutinas a comandos entendibles por el resto de nodos y los ejecuta.

          [`rviz_bridge/`](./src/robot_pkg/src/rviz_bridge_node.cpp) : Nodo puente entre el ros2 y rviz.

          [`servo_bridge/`](./src/robot_pkg/src/servo_bridge_node.cpp) : Nodo puente entre el ros2 y arduino (para controlar el robot fisico).
        
        - [`urdf/`](./src/robot_pkg/urdf/)  
          Archivo URDF del robot a usar

