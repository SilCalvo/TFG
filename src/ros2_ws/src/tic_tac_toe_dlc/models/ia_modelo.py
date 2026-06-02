import torch
import torch.nn as nn
from PIL import Image
import os
import torch.optim as optim
from torch.utils.data import DataLoader
from torchvision import datasets, transforms

from sklearn.metrics import confusion_matrix, precision_score, recall_score, f1_score, accuracy_score
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

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
        clases = ['-', 'o', 'x'] 
        
        nombre_clase = clases[indice.item()]
        porcentaje = confianza.item() * 100
        
        return nombre_clase, porcentaje

    except Exception as e:
        print(f"Error prediciendo {ruta_imagen}: {e}")
        return "error", 0.0

def generar_graficas_entrenamiento(historial_loss, historial_acc):
    epocas = range(1, len(historial_loss) + 1)
    
    plt.figure(figsize=(12, 5))
    
    # Gráfic Evolución del Loss (Pérdida)
    plt.subplot(1, 2, 1)
    plt.plot(epocas, historial_loss, 'r-', marker='o', markersize=4, label='Pérdida (Loss)')
    plt.title('Evolución de la Función de Pérdida', fontsize=12)
    plt.xlabel('Época', fontsize=10)
    plt.ylabel('Pérdida (CrossEntropy)', fontsize=10)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    
    # Gráfica Evolución de la Exactitud (Accuracy)
    plt.subplot(1, 2, 2)
    plt.plot(epocas, historial_acc, 'b-', marker='o', markersize=4, label='Exactitud (Accuracy)')
    plt.title('Evolución de la Exactitud', fontsize=12)
    plt.xlabel('Época', fontsize=10)
    plt.ylabel('Exactitud (%)', fontsize=10)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    
    plt.tight_layout()
    plt.savefig('graficas_entrenamiento.png', dpi=300)
    print("\nGráficas de entrenamiento guardadas como 'graficas_entrenamiento.png'")
    plt.show()

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

    print("Iniciando entrenamiento (5 épocas)...")
    
    historial_loss = []
    historial_acc = []
    
    for epoch in range(5):  
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
        epoch_loss = total_loss / len(dataloader)
        
        print(f"Epoch {epoch+1}/5 -> Loss: {epoch_loss:.4f} | Acc: {acc:.2f}%")
        
        historial_loss.append(epoch_loss)
        historial_acc.append(acc)

    torch.save(model_train.state_dict(), NOMBRE_MODELO)
    print(f"Modelo guardado en '{NOMBRE_MODELO}'")
    
    generar_graficas_entrenamiento(historial_loss, historial_acc)

def evaluar():
    print("\n--- GENERANDO TABLAS DE EVALUACIÓN ---")
    if not os.path.exists(RUTA_DATASET):
        print(f"Error: No encuentro la carpeta '{RUTA_DATASET}'")
        return

    dataset = datasets.ImageFolder(root=RUTA_DATASET, transform=transform)
    dataloader = DataLoader(dataset, batch_size=16, shuffle=False) 
    
    cargar_modelo(NOMBRE_MODELO)
    
    y_true = []
    y_pred = []

    print("Calculando predicciones...")
    with torch.no_grad():
        for inputs, labels in dataloader:
            inputs = inputs.to(DEVICE)
            outputs = model(inputs)
            _, predicted = torch.max(outputs, 1)
            
            y_true.extend(labels.cpu().numpy())
            y_pred.extend(predicted.cpu().numpy())

    # --- CÁLCULO DE MÉTRICAS ---
    acc = accuracy_score(y_true, y_pred) * 100
    precisiones = precision_score(y_true, y_pred, average=None, zero_division=0)
    sensibilidades = recall_score(y_true, y_pred, average=None, zero_division=0)
    f1_global = f1_score(y_true, y_pred, average='weighted')
    cm = confusion_matrix(y_true, y_pred)

    # --- IMPRESIÓN DE LA TABLA DE MÉTRICAS ---
    print("\nEvaluación del modelo")
    print("---------------------------------------------------------")
    print(f"{'Métrica':<35} | Valor Obtenido")
    print("---------------------------------------------------------")
    print(f"{'Exactitud Global (Accuracy)':<35} | {acc:.2f} %")
    print(f"{'Precisión - Clase Vacía (-)':<35} | {precisiones[0]:.4f}")
    print(f"{'Precisión - Clase O':<35} | {precisiones[1]:.4f}")
    print(f"{'Precisión - Clase X':<35} | {precisiones[2]:.4f}")
    print(f"{'Sensibilidad (Recall) - Clase (-)':<35} | {sensibilidades[0]:.4f}")
    print(f"{'Sensibilidad (Recall) - Clase O':<35} | {sensibilidades[1]:.4f}")
    print(f"{'Sensibilidad (Recall) - Clase X':<35} | {sensibilidades[2]:.4f}")
    print(f"{'F1-Score Global':<35} | {f1_global:.4f}")
    print("---------------------------------------------------------")

    # --- GENERAR Y GUARDAR LA GRÁFICA DE LA MATRIZ DE CONFUSIÓN ---
    plt.figure(figsize=(8, 6))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
                xticklabels=['Vacío (-)', 'O', 'X'], 
                yticklabels=['Vacío (-)', 'O', 'X'])
    
    plt.title('Matriz de Confusión - Reconocimiento Tres en Raya', fontsize=14)
    plt.ylabel('Etiqueta Real', fontsize=12)
    plt.xlabel('Predicción del Modelo', fontsize=12)
    
    plt.tight_layout()
    plt.savefig('grafica_matriz_confusion.png', dpi=300)
    print("\nGráfica guardada como 'grafica_matriz_confusion.png'")
    
    plt.show()

if __name__ == "__main__":
    entrenar()
    evaluar()
