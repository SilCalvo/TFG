# Requisitos del sistema

Para que SURI funcione correctamente, es necesario tener instalados los siguientes componentes en la máquina local. En caso de usar Windows, es posible ejecutarlo utilizando el subsistema de Linux (WSL2):

  - **Sistema Operativo:** Ubuntu 24.04 (Nativo o en WSL2).
  - **ROS 2 Jazzy:** El núcleo de comunicaciones de la plataforma.
  - **Pybullet:** Para el entorno del simulador de físicas.
  - **RViz2:** (Normalmente se instala automáticamente con la versión Desktop de ROS 2).
  - **Arduino IDE:** Necesario para compilar y subir el código al controlador del robot físico.


# Ejecutar Código:

```bash
# 1. Clonar el repositorio
git clone https://github.com/SilCalvo/TFG.git

# 2. Entrar en la carpeta del espacio de trabajo (workspace)
cd TFG/src/ros2_ws

# 3. Compilar los paquetes de ROS 2
colcon build

# 4. Cargar el entorno para que la terminal reconozca los comandos
source install/setup.bash

```

Nota: El paso 4 (source install/setup.bash) debe ejecutarse cada vez que se abra una terminal nueva, a menos que se añada al archivo .bashrc del sistema. 

Existen varias maneras de ejecutar el codigo: 
Solo virtual:

```bash
ros2 launch robot_pkg simulation.launch.py
```

Solo fisico:

```bash
ros2 launch robot_pkg robot_real.launch.py
```

Hibrido:

```bash
ros2 launch robot_pkg hibrid.launch.py
```

## Pestañas abiertas

Se abren 2 pestañas:
- **Rvizz**: Muestra el estado actual de los ejes del robot. Moviemiento acorde con los comandos enviados, si se usa un robot fisico, se moverá acorde al robot fisico tambien. Ajustalo a tu gusto.

Recomendacion: Añade el topic de TF ... y el topic de robot model, seleccionado en la pesataña de topic, el topic que aparezca. 

- **Pybullet**: Gemelo digital del robot. Este se ve un entorno que se puede observar las pareces virtuales, meter objetos en escena, camaras rgbd… se mueve al recibir los comandos que los recibe a la vez que el robot real.

