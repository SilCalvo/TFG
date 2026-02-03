#include <Servo.h>

Servo miServo;
String inputString = "";         // Para guardar los datos recibidos
boolean stringComplete = false;  // Flag para saber si llegó el mensaje

void setup() {
  Serial.begin(57600); // Velocidad rápida
  miServo.attach(9);   // Tu servo en el pin 9
  inputString.reserve(200);
  
  // Posición inicial
  miServo.write(90);
}

void loop() {
  // Si recibimos el comando completo (ej: "120\n")
  if (stringComplete) {
    int angulo = inputString.toInt(); // Convertir texto a numero
    
    // Limites de seguridad
    if(angulo > 180) angulo = 180;
    if(angulo < 0) angulo = 0;
    
    miServo.write(angulo);
    
    // Limpiar para la próxima
    inputString = "";
    stringComplete = false;
  }
}

// Evento que se dispara automáticamente cuando llegan datos seriales
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    // Si detecta un salto de línea, terminamos
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
