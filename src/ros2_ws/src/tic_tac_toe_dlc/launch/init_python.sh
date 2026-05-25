#!/bin/bash
echo "========================================="
echo "   INICIANDO CEREBRO IA (PYTORCH) "
echo "========================================="

if [ ! -d "ia_env" ]; then
    echo "[INFO] Creando entorno virtual 'ia_env'..."
    python3 -m venv ia_env
    source ia_env/bin/activate
    
    pip install --upgrade pip
    pip install torch torchvision --index-url https://download.pytorch.org/whl/cpu
    pip install numpy Pillow pyyaml typeguard scikit-learn matplotlib seaborn
    
    touch ia_env/COLCON_IGNORE
    source ia_env/bin/activate
else
    source ia_env/bin/activate
fi

source /opt/ros/jazzy/setup.bash
source install/setup.bash

python3 src/tic_tac_toe_dlc/models/ia_server.py
