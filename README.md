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
    Código final del brazo robotico en arduino. Para más informació [`arduino.md/`](./src/arduino/arduino.md)

  - [`pybullet/`](./src/pybullet/)  
    Codigo de prueba para comprpbar que la cinematica directa calculada mediante el metodo denavit hartenberg es correcta comprobandolo con pybullet.Para más informació [`pybullet.md/`](./src/pybullet/pybullet.md)

  - [`ros2_ws/`](./src/ros2_ws/)  
   Workspace en ros2 jazzy con todos los paquetes necesarios para el funcionamiento. Para más informació [`ros2_ws.md/`](./src/ros2_ws/ros2_ws.md)

- [`documentacion/`](./doc/)  
 Contiene la documentación de este programa (Como camiar un robot, como funciona, manuales, informes... ) Para más informació [`doc.md/`](./doc/./doc.md)

Para más información sobre el proyecto acuda a [`documentacion/`](./doc/) 

## ----------------------- TEMPORAL para probar ahora -----------------------

## Como funciona:
1. Compilar workspace
2. Hacer launch del paquete robot_pkg 
3. Eviar una petición de accion a un punto en x,y,z
4. Calcula la cinematica inversa con pybullet (es un servicio)
5. Se mueve a la posision y se puede ver mediante rviz2, en pybullet se ve las pruebas que hace para llegar a los objetivos y las herramientas que se vayan seleccionado. 


#### Por que pybullet
Para cambiar el robot solo hay que cambiar el urdf , la cinemática directa y en el nodo rviz_bridge cambiar a los nuevos nombres. De esta manera el software es lo más indepediente posible al robot fisico y cambiarlo no supondía más de 2 minutos.





## FALTA

Falta probar comunicacion arduino --> pc

Falta: ----------------------------------------

- Current angles en move_node en get trayectory que los coja bien. 
- Falta lo de +90 grados al enviar al arduino
- Añadir paredes virtuales - manual 
- Añadir TCP - manual 
- Juego de 3 en raya real 
- Juego 3 en raya virtual mover robot
- URDF del robot real
- Manual de desarrolador
- Manual de usuario
- Cinematica directa introducirlo de alguna manera
- Limpiar codigo

- 
