## Arduino

Código para controlar el robot físico medinate una placa Arduino. 

Dos modos:
1. **Automático**: Es controlado por comandos de Ros2
2. **Manual:**: Es controlado por Arduino (potenciómetros)

### Estructura 

- [`control_robot/`](./control_robot/)  
  Carpeta con los códigos arduino.

    - [`control_robot.ino/`](./control_robot/control_robot.ino)  
      Código principal de arduino, controla el modo manual y automatico.

    - [`robot_automatico.ino/`](./control_robot/robot_automatico.ino)  
      Código del control automático del robot (controlado por ros2).
  
    - [`robot_manual.ino/`](./control_robot/robot_manual.ino)  
      Código del control manual del robot (controlado con los potenciómetros fisicos).

**Borrar carpeta CONTROL** Es solo una prueba
  