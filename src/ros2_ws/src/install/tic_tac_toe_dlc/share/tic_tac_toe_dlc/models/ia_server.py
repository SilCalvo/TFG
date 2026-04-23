import rclpy
from rclpy.node import Node
from std_srvs.srv import Trigger
import torch
from PIL import Image
import os

from ia_modelo import CNN, transform, DEVICE, NOMBRE_MODELO

class IAServer(Node):
    def __init__(self):
        super().__init__('ia_server_node')
        
        self.srv = self.create_service(Trigger, '/analizar_tablero', self.analizar_callback)
        
        self.model = CNN().to(DEVICE)
        directorio_actual = os.path.dirname(os.path.abspath(__file__))
        path = os.path.join(directorio_actual, NOMBRE_MODELO)
        if os.path.exists(path):
            self.model.load_state_dict(torch.load(path, map_location=DEVICE))
            self.model.eval()
            self.get_logger().info(f"¡Cerebro PyTorch cargado desde {path}! Esperando imágenes...")
        else:
            self.get_logger().error(f"Error crítico: No encuentro el modelo en la ruta {path}")
    def analizar_callback(self, request, response):
        self.get_logger().info("¡C++ me ha llamado! Analizando las 9 casillas...")
        clases = ['-', 'o', 'x']
        resultado = []
        
        for i in range(9):
            ruta = f"/tmp/casilla_{i}.jpg"
            try:
                img = Image.open(ruta)
                img_tensor = transform(img).unsqueeze(0).to(DEVICE)
                
                with torch.no_grad():
                    output = self.model(img_tensor)
                    _, indice = torch.max(output[0], 0)
                    resultado.append(clases[indice.item()])
            except Exception as e:
                self.get_logger().error(f"Error leyendo {ruta}: {e}")
                resultado.append('-')
        
        response.success = True
        response.message = ",".join(resultado) 
        self.get_logger().info(f"Análisis completado: {response.message}")
        
        return response

def main(args=None):
    rclpy.init(args=args)
    node = IAServer()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()