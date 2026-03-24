# TFG


### Arduino: 
Progama del brazo robotico en arduino

### Pybullet: 
Codigo de prueba para comprpbar que la cinematica directa calculada mediante el metodo denavit hartenberg es correcta comprobandolo con pybullet (URDF de prueba, es uno que hice en enero para acordarme de como calcular cinematica directa)

### ros2_ws:
workspace de ros2 Jazzy. 

#### Como funciona:
1. Hacer launch del paquete robot_pkg:
2. Eviar una petición de accion a un punto en x,y,z:
3. Calcula la cinematica inversa con pybullet (es un servicio)
4. Se mueve a la posision y se puede ver mediante rviz2. 

#### Por que pybullet
Para cambiar el robot solo hay que cambiar el urdf y en el nodo rviz_bridge cambiar a los nuevos nombres. De esta manera el software es lo más indepediente posible al robot fisico y cambiarlo no supondía más de 2 minutos.


## FALTA

Falta probar comunicacion arduino --> pc

Falta: ----------------------------------------


Cinematica directa bien


Añadir paredes virtuales - manual y automatico


Añadir TCP - manual y automatico


Juego de 3 en raya / vision ....


URDF del robot
