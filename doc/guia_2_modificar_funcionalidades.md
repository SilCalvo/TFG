
# GUIA 2 MODIFICAR FUNCIONALIDADES

A conticacion se muestra la lista de comandos para cada funcionalidad.



# COMANDOS

## Movimientos del Robot

* **`moveJ`**: Movimiento articular a una pose cartesiana.
  * 
```bash
ros2 action send_goal /moveJ nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base'}, pose: {position: {x: -0.4, y: 0.0, z: 0.3}, orientation: {x: 0.0, y: 1.0, z: 0.0, w: 0.0}}}}"
```
* **`moveL`**: Movimiento lineal cartesiano a una pose.
  * 
```bash
ros2 action send_goal /moveL nav2_msgs/action/NavigateToPose "{pose: {header: {frame_id: 'base'}, pose: {position: {x: 0.3, y: 0.0, z: 1.1}, orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}}}}"
```
* **`control_joint`**: Mueve una articulación concreta a un ángulo.
  
```bash
ros2 service call /control_joint robot_interfaces/srv/MoveJoint "{index: 2, degrees: 45.0}"

```
* **`go_home`**: Devuelve el robot a la posición de reposo.
  * 
```bash
ros2 service call /go_home robot_interfaces/srv/MoveJoint "{index: 0, degrees: 0.0}"

```

## Entorno y Colisiones

* **`add_wall`**: Crea una pared virtual de colisión.
  
```bash
ros2 service call /add_wall robot_interfaces/srv/AddObstacle "{                                                                                                                                        
  name: 'pared_frontal',
  x: 0.5,
  y: 0.0,
  z: 0.0,
  roll: 0.0,
  pitch: 0.0,
  yaw: 0.0,
  width: 0.02,
  height: 3.0,
  depth: 0.8
}"
* **`remove_wall`**: Elimina una pared virtual existente.
  * 
```bash
ros2 service call /remove_wall robot_interfaces/srv/RemoveObstacle "{name: 'pared_frontal'}"
```

## Configuración de Herramientas (TCP)

* **`add_tool`**: Define un nuevo TCP (herramienta).
  
```bash
ros2 service call /add_tool robot_interfaces/srv/ManageTool "{name: 'pinza', type: 1, dimensions: [0.11, 0.05], offset: {position: {z: 0.0}, orientation: {w: 1.0}}}"
```
* **`delete_tool`**: Elimina la definición de una herramienta.
  * 
```bash
ros2 service call /add_tool robot_interfaces/srv/ManageTool "{name: 'pinza', type: 1, dimensions: 0, offset: 0"
```
* **`set_tool`**: Cambia la herramienta activa del sistema.
  * 
```bash
ros2 param set /move_action_server active_tool "pinza"
```


# RUTINAS

las rutinas nuevas se debe nguardar en robot_ws/src/robot_pkg/rutines en un archivo .yaml

## Estructura de una rutina 
```bash
rutina:
  - tipo: "moveJ"
    x: -0.4
    y: 0.0
    z: 0.3
    qx: 0.0
    qy: 1.0
    qz: 0.0
    qw: 0.0

  - tipo: "go_home"
    index: 0      
    degrees: 0.0
```

## Movimientos del Robot

* **`moveJ`**: Movimiento articular a una pose cartesiana.
  * **Parámetros:** `x`, `y`, `z`, `qx`, `qy`, `qz`, `qw`
* **`moveL`**: Movimiento lineal cartesiano a una pose.
  * **Parámetros:** `x`, `y`, `z`, `qx`, `qy`, `qz`, `qw`
* **`control_joint`**: Mueve una articulación concreta a un ángulo.
  * **Parámetros:** `index`, `degrees`
* **`go_home`**: Devuelve el robot a la posición de reposo.
  * **Parámetros:** *Sin parámetros*

## Entorno y Colisiones

* **`add_wall`**: Crea una pared virtual de colisión.
  * **Parámetros:** `name`, `x`, `y`, `z`, `width`, `depth`, `height`
* **`remove_wall`**: Elimina una pared virtual existente.
  * **Parámetros:** `name`

## Configuración de Herramientas (TCP)

* **`add_tool`**: Define un nuevo TCP (herramienta).
  * **Parámetros:** `name`, `tool_type`, `dimensions`, `off_x`, `off_y`, `off_z`, `off_qw`
* **`delete_tool`**: Elimina la definición de una herramienta.
  * **Parámetros:** `name`
* **`set_tool`**: Cambia la herramienta activa del sistema.
  * **Parámetros:** `name`

