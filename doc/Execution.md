# Requisitos del sistema

Debes tener todos ellos instalados en tu máquina local. 
- LINUX UBUNTU 2024
- Ros2 Jazzy
- Pybullet
- Rvizz
- Arduino

# Ejecutar Código:

```bash
git clone <repo_url>
cd TFG/src/ros2_ws
colcon build
```
```bash
ros2 launch robot_pkg move_action.launch.py
```
## Pestañas abiertas

Se abren 2 pestañas:
- **Rvizz**: Muestra el estado actual de los ejes del robot. Moviemiento acorde con los comandos enviados, si se usa un robot fisico, se moverá acorde al robot fisico tambien. Ajustalo a tu gusto.

Recomendacion: Añade el topic de TF ... y el topic de robot model, seleccionado en la pesataña de topic, el topic que aparezca. 

- **Pybullet**: Gemelo digital del robot...

## Modos de Uso

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


### Mover una articulacion

Sirve para mover una articulación a un grado dado, como por ejemplo cerrar la pinza que sea x grados

**COMANDOS**

```bash
ros2 service call /control_joint robot_interfaces/srv/MoveJoint "{index: 2, degrees: 45.0}"

```

**RUTINA**

```bash
ros2 run robot_pkg rutine_node --ros-args -p archivo:="test_move_articulations.yaml"
```


### Mover robot a posicion segura HOME

Coloca al robot en posision segura home 

**COMANDOS**

```bash
ros2 service call /go_home robot_interfaces/srv/MoveJoint "{index: 0, degrees: 0.0}"

```

**RUTINA**

```bash
ros2 run robot_pkg rutine_node --ros-args -p archivo:="test_go_home.yaml"
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
  roll: 0.0,
  pitch: 0.0,
  yaw: 0.0,
  width: 0.02,
  height: 3.0,
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

### Jugar al 3 en raya

**COMANDOS**

__Añadir camara__
```bash

ros2 service call /add_camera robot_interfaces/srv/AddCamera "{name: 'camara_tablero', x: 0.35, y: 0.01, z: 0.45, target_x: 0.35, target_y: 0.0, target_z: 0.0, width: 640, height: 480}"

```

__Añadir tablero__
```bash

ros2 service call /add_object robot_interfaces/srv/SpawnObject "{name: 'tablero_principal', package_name: 'tic_tac_toe_dlc', piece_type: 'tablero', x: 0.35, y: 0.0, z: 0.0}"
```

__Añadir entorno y ML__

Crear entorno para modelo de aprendizaje automatico con pytorch (ocupa 1 GB aprox).

En una terminal diferente escribe:
```bash

chmod +x src/tic_tac_toe_dlc/launch/init_python.sh
./src/tic_tac_toe_dlc/launch/init_python.sh
```

__Lanzar juego__

En otra terminal diferente:
```bash
ros2 launch tic_tac_toe_dlc init_game.launch.py 
```

__Colocar ficha__

Para poner una ficha puedes usar este servicio indicando la fila y la columna donde quieras poner tu ficha
```bash
ros2 service call /put_piece_virtual robot_interfaces/srv/PutPiece "{fila: 1, columna: 1}"
```

O puedes poner a mano tu ficha si sabes en que coordernadas ponerla
```bash
ros2 service call /add_object robot_interfaces/srv/SpawnObject "{name: 'ficha_o_1', package_name: 'tic_tac_toe_dlc', piece_type: 'ficha_o', x: 0.35, y: 0.0, z: 0.03}"

```


### Funcionalidades
**Añadir TCP**

**Selecionar TCP**

**Borrar TCP**

**Añadir pared**

**Borrar pared**

**Ejecutar rutina**

**Añadir camara**

**Borrar camara**

**Añadir objeto**

**Borrar objeto**
