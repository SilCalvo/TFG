# TFG


### Arduino: 
Progama del brazo robotico en arduino

### Pybullet: 
Codigo de prueba para comprpbar que la cinematica directa calculada mediante el metodo denavit hartenberg es correcta comprobandolo con pybullet (URDF de prueba, es uno que hice en enero para acordarme de como calcular cinematica directa)

### ros2_ws:
workspace de ros2 Jazzy. 

#### Como funciona:
1. Hacer launch del paquete robot_pkg: ros2 launch robot_pkg move_action.launch.py
2. Eviar una petición de accion a un punto en x,y,z: 

	2.a Puede ser *MOVEJ O MOVEL*: 
	
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.4, y: 0.0, z: 0.5}, orientation: {w: 1.0}}}}"

ros2 action send_goal /moveL nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.3, y: 0.3, z: 0.6}, orientation: {w: 1.0}}}}"

3. Calcula la cinematica inversa con pybullet (es un servicio)
4. Se mueve a la posision y se puede ver mediante rviz2. 

#### Por que pybullet
Para cambiar el robot solo hay que cambiar el urdf , la cinemática directa y en el nodo rviz_bridge cambiar a los nuevos nombres. De esta manera el software es lo más indepediente posible al robot fisico y cambiarlo no supondía más de 2 minutos.


## FALTA

Falta probar comunicacion arduino --> pc

Falta: ----------------------------------------

Current angles en move_node en get trayectory que los coja bien. 

Comprobar que pasa si se envia a un punto al que no llega

Falta lo de +90 grados al enviar al arduino

Añadir paredes virtuales - manual y automatico


Añadir TCP - manual y automatico


Juego de 3 en raya / vision ....


URDF del robot con su Cinematica directa bien
