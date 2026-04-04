# TFG


### Arduino: 
Progama del brazo robotico en arduino

### Pybullet: 
Codigo de prueba para comprpbar que la cinematica directa calculada mediante el metodo denavit hartenberg es correcta comprobandolo con pybullet 

### ros2_ws:
workspace de ros2 Jazzy. 

#### Como funciona:
1. Hacer launch del paquete robot_pkg: ros2 launch robot_pkg move_action.launch.py
2. Eviar una petición de accion a un punto en x,y,z
3. Calcula la cinematica inversa con pybullet (es un servicio)
4. Se mueve a la posision y se puede ver mediante rviz2, en pybullet se ve las pruebas que hace para llegar a los objetivos y las herramientas que se vayan seleccionado. 

#### Probar Códido:

*Probar Move J y Move L con TCP default*

ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: -0.4, y: 0.0, z: 0.3}, orientation: {x: 0.0, y: 1.0, z: 0.0, w: 0.0}}}}"

ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: -0.2, y: 0.0, z: 1.4}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}}}"


ros2 action send_goal /moveL nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.3, y: 0.0, z: 1.4}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}}}"


*Probar TCP nuevo*

ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: -0.4, y: 0.0, z: 0.3}, orientation: {x: 0.0, y: 1.0, z: 0.0, w: 0.0}}}}"


ros2 service call /add_tool robot_interfaces/srv/ManageTool "{name: 'pinza', type: 1, dimensions: [0.1, 0.05], offset: {position: {z: 0.0}, orientation: {w: 1.0}}}"


ros2 param set /move_action_server active_tool "pinza"



ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: -0.4, y: 0.0, z: 0.3}, orientation: {x: 0.0, y: 1.0, z: 0.0, w: 0.0}}}}"


Se puede observar como el brazo baja la diferencia de 0.15 que es la diferencia del tamaño de las herramientas.


*Probar añadir pared virtual*


ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.7, y: 0.0, z: 0.9}, orientation: {x: 0.707, y: 0.0, z: 0.707, w: 0.0}}}}"



ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.0, y: 0.0, z: 1.4}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}}}"


ros2 service call /add_wall robot_interfaces/srv/AddObstacle "{                             
  name: 'pared_frontal',
  x: 0.5,
  y: 0.0,
  z: 0.0,
  width: 0.02,
  height: 3,
  depth: 0.8
}"

ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.7, y: 0.0, z: 0.9}, orientation: {x: 0.707, y: 0.0, z: 0.707, w: 0.0}}}}"



ros2 service call /remove_wall robot_interfaces/srv/RemoveObstacle "{name: 'pared_frontal'}"



#### Por que pybullet
Para cambiar el robot solo hay que cambiar el urdf , la cinemática directa y en el nodo rviz_bridge cambiar a los nuevos nombres. De esta manera el software es lo más indepediente posible al robot fisico y cambiarlo no supondía más de 2 minutos.





## FALTA

Falta probar comunicacion arduino --> pc

Falta: ----------------------------------------

Current angles en move_node en get trayectory que los coja bien. 


Falta lo de +90 grados al enviar al arduino

Añadir paredes virtuales - manual 


Añadir TCP - manual 


Juego de 3 en raya / vision ....


URDF del robot con su Cinematica directa bien
