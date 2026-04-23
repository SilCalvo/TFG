import torch
import torch.nn as nn
from PIL import Image
import os
import torch.optim as optim
from torch.utils.data import DataLoader
from torchvision import datasets, transforms

# --- CONFIGURACIÓN ---
DEVICE = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
RUTA_DATASET = './train' 
NOMBRE_MODELO = 'modelo_tictactoe.pth'

# --- TRANSFORMACIONES ---
# Para entrenar 
transform_train = transforms.Compose([
    transforms.Grayscale(num_output_channels=1),
    transforms.Resize((28, 28)),
    transforms.RandomRotation(15),      
    transforms.RandomAffine(0, translate=(0.1, 0.1)), 
    transforms.ColorJitter(brightness=0.2, contrast=0.2), 
    transforms.ToTensor(),
    transforms.Normalize((0.5,), (0.5,))
])

# Para predecir 
transform = transforms.Compose([
    transforms.Grayscale(num_output_channels=1),
    transforms.Resize((28, 28)),
    transforms.ToTensor(),
    transforms.Normalize((0.5,), (0.5,))
])

# --- MODELO CNN ---
class CNN(nn.Module):
    def __init__(self):
        super().__init__()
        self.features = nn.Sequential(
            nn.Conv2d(1, 32, kernel_size=3, padding=1),
            nn.ReLU(),
            nn.MaxPool2d(2),
            nn.Conv2d(32, 64, kernel_size=3, padding=1),
            nn.ReLU(),
            nn.MaxPool2d(2),
        )
        self.classifier = nn.Sequential(
            nn.Flatten(),
            nn.Linear(64 * 7 * 7, 128),
            nn.ReLU(),
            nn.Dropout(0.5), # Apaga neuronas al azar para evitar memorización
            nn.Linear(128, 3) 
        )

    def forward(self, x):
        x = self.features(x)
        x = self.classifier(x)
        return x


model = CNN().to(DEVICE)
loaded = False

def cargar_modelo(path=NOMBRE_MODELO):
    global loaded
    if os.path.exists(path):
        model.load_state_dict(torch.load(path, map_location=DEVICE))
        model.eval()
        loaded = True
        print(f"Modelo IA cargado desde {path}")
    else:
        print(f"Error: No se encuentra el modelo en {path}")

def predecir_imagen(ruta_imagen):
    if not loaded: return "error", 0.0
    
    try:
        img = Image.open(ruta_imagen)
        img_tensor = transform(img).unsqueeze(0).to(DEVICE)
        
        with torch.no_grad():
            output = model(img_tensor)
            probs = torch.nn.functional.softmax(output[0], dim=0)
            confianza, indice = torch.max(probs, 0)
            
        # --- LISTA FIJA (HARDCODED) ---
        # Asumiendo carpetas: 'nada', 'o', 'x' (orden alfabético)
        clases = ['-', 'o', 'x'] 
        
        nombre_clase = clases[indice.item()]
        porcentaje = confianza.item() * 100
        
        return nombre_clase, porcentaje

    except Exception as e:
        print(f"Error prediciendo {ruta_imagen}: {e}")
        return "error", 0.0

def entrenar():
    if not os.path.exists(RUTA_DATASET):
        print(f"Error: No encuentro la carpeta '{RUTA_DATASET}'")
        return

    print("Cargando imágenes...")
    dataset = datasets.ImageFolder(root=RUTA_DATASET, transform=transform_train)
    
    print(f"ORDEN DETECTADO (Asegúrate que coincide con ['-', 'o', 'x']):")
    print(dataset.class_to_idx) 

    dataloader = DataLoader(dataset, batch_size=16, shuffle=True)
    
    model_train = CNN().to(DEVICE)
    model_train.train()
    
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model_train.parameters(), lr=0.001)

    print("Iniciando entrenamiento (20 épocas)...")
    
    for epoch in range(35): 
        total_loss = 0
        correct = 0
        total = 0
        
        for inputs, labels in dataloader:
            inputs, labels = inputs.to(DEVICE), labels.to(DEVICE)
            
            optimizer.zero_grad()
            outputs = model_train(inputs)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()
            
            total_loss += loss.item()
            _, predicted = torch.max(outputs, 1)
            total += labels.size(0)
            correct += (predicted == labels).sum().item()
            
        acc = 100 * correct / total
        print(f"Epoch {epoch+1}/20 -> Loss: {total_loss/len(dataloader):.4f} | Acc: {acc:.2f}%")

    torch.save(model_train.state_dict(), NOMBRE_MODELO)
    print(f"Modelo guardado en '{NOMBRE_MODELO}'")

if __name__ == "__main__":
    entrenar()