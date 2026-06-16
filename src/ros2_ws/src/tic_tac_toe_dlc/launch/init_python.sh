#!/bin/bash
echo "========================================="
echo "   INICIANDO CEREBRO IA (PYTORCH) "
echo "========================================="

if [ ! -d "ia_env" ]; then
    echo "[INFO] Creando entorno virtual 'ia_env'..."
    python3 -m venv --system-site-packages ia_env
    source ia_env/bin/activate
    
    echo "[INFO] Actualizando pip..."
    pip install --upgrade pip --index-url https://pypi.org/simple/
    
    echo "[INFO] Instalando PyTorch..."
    pip install torch torchvision --index-url https://download.pytorch.org/whl/cpu
    
    echo "[INFO] Instalando dependencias básicas y parches..."
    pip install joblib pyyaml typeguard --index-url https://pypi.org/simple/
    
    echo "[INFO] Instalando resto de librerías de IA..."
    pip install numpy Pillow scikit-learn matplotlib seaborn --index-url https://pypi.org/simple/
    
    touch ia_env/COLCON_IGNORE
else
    source ./ia_env/bin/activate
fi

source /opt/ros/jazzy/setup.bash
source install/setup.bash
source ./ia_env/bin/activate

python3 src/tic_tac_toe_dlc/models/ia_server.py