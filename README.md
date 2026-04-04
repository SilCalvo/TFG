#  Plataforma de Simulación Universal para Robótica Industrial (SURI) -  TFG Silvia Calvo Cabello

## Descripción

Es un entorno de simulación basado en ROS 2 y PyBullet diseñado para generar el gemelo digital de cualquier manipulador robótico industrial. Su arquitectura es completamente independiente del hardware (hardware-agnostic), permitiendo validar trayectorias y tareas en diferentes robots con tan solo proporcionar su modelo descriptivo URDF.

Enfoque dual:
- *Para el Usuario u Operador*: Proporciona una interfaz de alto nivel enfocada en la usabilidad. Permite enviar comandos intuitivos (movimiento de coordenadas, control de pinzas) y visualizar la ejecución en tiempo real sin necesidad de un alto conocimento en robótica.

- *Para el Programador*: Ofrece una base de código modular diseñada para ser ampliada. Permite desde programar e integrar nuevas funcionalidades a medida, hasta cambiar el modelo del robot por completo de forma ágil y sin reestructurar el sistema, adaptándose a las necesidades de cualquier proyecto.

## Estructura del Repositorio

En las siguientes carpetas encontrarás todos los recursos del proyecto:

- [`src/`](./src/)  
  Contiene todas las capetas con los diferentes código de este proyecto. Dentro de esta carpeta se incluyen:

  - [`Arduino/`](./src/arduino/)  
    Código final del brazo robotico en arduino. Para más informació [`arduino md/`](./src/arduino/arduino.md)

  - [`pybullet/`](./src/pybullet/)  
    Codigo de prueba para comprpbar que la cinematica directa calculada mediante el metodo denavit hartenberg es correcta comprobandolo con pybullet.Para más informació [`pybullet md/`](./src/pybullet/pybullet.md)

  - [`ros2_ws/`](./src/ros2_ws/)  
   Workspace en ros2 jazzy con todos los paquetes necesarios para el funcionamiento. Para más informació [`ros2_ws md/`](./src/ros2_ws/ros2_ws.md)

- [`documentacion/`](./doc/)  
 Contiene la documentación de este programa (Como camiar un robot, como funciona, manuales, informes... )


## ----------------------- TEMPORAL para probar ahora -----------------------
## Requisitos

- Ros2 Jazzy
- Pybullet
- Rvizz
- Arduino


## Como funciona:
1. Compilar workspace
2. Hacer launch del paquete robot_pkg 
3. Eviar una petición de accion a un punto en x,y,z
4. Calcula la cinematica inversa con pybullet (es un servicio)
5. Se mueve a la posision y se puede ver mediante rviz2, en pybullet se ve las pruebas que hace para llegar a los objetivos y las herramientas que se vayan seleccionado. 

### Probar Códido:

```bash
colcon build
```
```bash
ros2 launch robot_pkg move_action.launch.py
```
Existen 2 modos de probarlo:

- **Comandos**: Escribiendo por terminal los comandos 
- **Rutina**: Ejecutando una rutina ya escrita


### Probar Move J y Move L con TCP default

**COMANDOS**

```bash
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: -0.4, y: 0.0, z: 0.3}, orientation: {x: 0.0, y: 1.0, z: 0.0, w: 0.0}}}}"
```
```bash
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: -0.2, y: 0.0, z: 1.4}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}}}"
```
```bash
ros2 action send_goal /moveL nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.3, y: 0.0, z: 1.4}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}}}"
```
**RUTINA**

```bash
ros2 run robot_pkg rutine_node
```

### Probar TCP nuevo

**COMANDOS**

```bash
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: -0.4, y: 0.0, z: 0.3}, orientation: {x: 0.0, y: 1.0, z: 0.0, w: 0.0}}}}"
```
```bash
ros2 service call /add_tool robot_interfaces/srv/ManageTool "{name: 'pinza', type: 1, dimensions: [0.1, 0.05], offset: {position: {z: 0.0}, orientation: {w: 1.0}}}"
```
```bash
ros2 param set /move_action_server active_tool "pinza"
```

```bash
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: -0.4, y: 0.0, z: 0.3}, orientation: {x: 0.0, y: 1.0, z: 0.0, w: 0.0}}}}"
```

Se puede observar como el brazo baja la diferencia de 0.15 que es la diferencia del tamaño de las herramientas.

**RUTINA**
```bash
ros2 run robot_pkg rutine_node --ros-args -p archivo:="test_tcp.yaml"
```

Se puede observar como el brazo baja la diferencia de 0.15 que es la diferencia del tamaño de las herramientas.


### Probar añadir pared virtual

Aqui habrá una diferencia porque la rutina da error ya que no podrá moverse por la pared virtual y no seguirá ejecutandose. Mientras que por comandos puedes seguir enviando el siguiente

**COMANDOS**

```bash
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.7, y: 0.0, z: 0.9}, orientation: {x: 0.707, y: 0.0, z: 0.707, w: 0.0}}}}"
```

```bash
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.0, y: 0.0, z: 1.4}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}}}"
```

```bash
ros2 service call /add_wall robot_interfaces/srv/AddObstacle "{                             
  name: 'pared_frontal',
  x: 0.5,
  y: 0.0,
  z: 0.0,
  width: 0.02,
  height: 3,
  depth: 0.8
}"
```

Puedes obbservar la pared virtual en pybullet 


```bash
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.7, y: 0.0, z: 0.9}, orientation: {x: 0.707, y: 0.0, z: 0.707, w: 0.0}}}}"
```

```bash
ros2 service call /remove_wall robot_interfaces/srv/RemoveObstacle "{name: 'pared_frontal'}"
```
```bash
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base_link'}, pose: {position: {x: 0.7, y: 0.0, z: 0.9}, orientation: {x: 0.707, y: 0.0, z: 0.707, w: 0.0}}}}"
```

**RUTINA**
```bash
ros2 run robot_pkg rutine_node --ros-args -p archivo:="test_wall.yaml"
```


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
