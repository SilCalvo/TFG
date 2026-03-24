import pybullet as p
import pybullet_data
import time
import numpy as np

# --- 1. CONFIGURACIÓN INICIAL ---
physicsClient = p.connect(p.GUI)
p.setAdditionalSearchPath(pybullet_data.getDataPath())
p.setGravity(0, 0, -9.8)
p.setRealTimeSimulation(True)

# Cargar plano y robot
planeId = p.loadURDF("plane.urdf")
startPosition_bloque = [0, 0, 0]
startOrientation_bloque = p.getQuaternionFromEuler([0, 0, 0])

# Cargar el robot (Asegúrate de que el URDF está en la carpeta)
robotId = p.loadURDF("robot.urdf", startPosition_bloque, startOrientation_bloque, useFixedBase=1)

# --- 2. DETECCIÓN DE JOINTS Y LINK FINAL ---
active_joints = [] 
end_effector_index = -1 # Variable para guardar el ID de la mano
nombre_link_final ="mano"# "c5_muneca2_gira"#"mano" # El nombre que pusiste en tu URDF

num_joints = p.getNumJoints(robotId)
print(f"Total joints/links detectados: {num_joints}")

for i in range(num_joints):
    joint_info = p.getJointInfo(robotId, i)
    # joint_info[1] es el nombre del Joint
    # joint_info[12] es el nombre del Link hijo (Child Link Name)
    joint_name = joint_info[1].decode("utf-8")
    link_name = joint_info[12].decode("utf-8")
    joint_type = joint_info[2]
    
    # Buscamos el índice del eslabón final "mano"
    if link_name == nombre_link_final:
        end_effector_index = i
        print(f"-> Eslabón final '{nombre_link_final}' encontrado en índice: {i}")

    # Configurar sliders solo para joints móviles
    if joint_type != p.JOINT_FIXED:
        print(f"Creando slider para: {joint_name} (ID: {i})")
        slider_id = p.addUserDebugParameter(joint_name, -1.57, 1.57, 0)
        active_joints.append((i, slider_id))

if end_effector_index == -1:
    print("¡ADVERTENCIA! No se encontró el link llamado 'mano'. Se usará el último link.")
    end_effector_index = num_joints - 1

print("Iniciando control manual...")

# --- 3. BUCLE DE SIMULACIÓN ---
try:
    while True:
        
        # A. Leer sliders y mover el robot
        for joint_index, slider_id in active_joints:
            target_pos = p.readUserDebugParameter(slider_id)
            p.setJointMotorControl2(
                bodyUniqueId=robotId,
                jointIndex=joint_index,
                controlMode=p.POSITION_CONTROL,
                targetPosition=target_pos,
                force=500
            )
        
        # B. Calcular Posición Final (Cinemática Directa)
        # getLinkState devuelve varias cosas. El índice [4] es la posición del marco del link (World Position)
        # El índice [5] es la orientación (World Orientation)
        link_state = p.getLinkState(robotId, end_effector_index)
        pos_xyz = link_state[4] 
        
        # C. Mostrar texto en pantalla (Debug Text)
        # Formateamos a 3 decimales
        texto_pos = f"X: {pos_xyz[0]:.3f}\nY: {pos_xyz[1]:.3f}\nZ: {pos_xyz[2]:.3f}"
        
        # Imprimimos en la simulación flotando sobre la mano
        # Usamos replaceItemUniqueId o lifeTime corto para que no se acumulen textos
        p.addUserDebugText(
            text=texto_pos,
            textPosition=[pos_xyz[0], pos_xyz[1], pos_xyz[2] + 0.2], # Un poco más arriba de la mano
            textColorRGB=[0, 0, 0], # Texto negro
            textSize=1.2,
            lifeTime=0.1 # Dura 0.1 segundos, luego desaparece (crea efecto de actualización)
        )

        time.sleep(1./240.)

except KeyboardInterrupt:
    p.disconnect()