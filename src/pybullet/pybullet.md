## Pybullet + Matlab

Comprobar cáculo de cinematica directa usando el método denavit hartenberg.


### Estructura

- [`directa/`](./Directa_v3.m)  
  Codigo para usar en matlab. Calcula la matriz final.

- [`comprobacion/`](./comprobacion.py)  
  Código que muestra la posición real del end effector y la muestra en el simulador

- [`robot/`](./robot.urdf)  
  URDF del robot

### Uso

Calcular las diferentes matrices de paso usando el método denavit hartenberg y comprobar que se obtiene el mismo punto final del end effector en matlab y en pybullet.




  
