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
    Código final del brazo robotico en arduino. Para más información [`arduino.md/`](./src/arduino/arduino.md)

  - [`pybullet/`](./src/pybullet/)  
    Codigo de prueba para comprobar que la cinemática directa calculada mediante el método denavit hartenberg es correcta comprobandolo con pybullet. Para más información [`pybullet.md/`](./src/pybullet/pybullet.md)

  - [`ros2_ws/`](./src/ros2_ws/)  
   Workspace en ros2 jazzy con todos los paquetes necesarios para el funcionamiento. Para más información [`ros2_ws.md/`](./src/ros2_ws/ros2_ws.md)

- [`documentacion/`](./doc/)  
 Contiene la documentación de este programa (Manual de uso, esquemas, videos... ) Para más información [`doc.md/`](./doc/doc.md)

Para más información sobre el proyecto así como los primeros pasos acuda a [`documentacion/`](./doc/) 


