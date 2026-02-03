// ================================================================
// --- AUTOMATIC CONTROL MODE ---
// ================================================================
/*
 * Receives angle commands via Serial communication (from ROS2).
 * Parses the incoming integer stream, applies safety limits (0-180), and updates the servo positions accordingly.
 */

void automatic_control(){

  int target_position[5]= {0,0,0,0,0};
  if (Serial.available() > 0) {
    for (int i = 0; i < 5; i++) {
      int lectura = Serial.parseInt();
      target_position[i] = lectura;
      
      // Security limits
      if(target_position[i] > 180) target_position[i] = 180;
      if(target_position[i] < 0) target_position[i] = 0;
    }
    
    // Clean Serial
    while(Serial.available()) { Serial.read(); }

    // Move servos
    for(int i=0; i<5; i++){
      servos[i].write(target_position[i]);
      actual_position[i] = target_position[i];
      delay(10); ////////////////////////////////////////////////////////////////////////////////
    }

  }
  delay(15);

}
/*
  if (stringComplete) {
    // --- PARSEO MANUAL "v1,v2,v3,v4,v5" ---
    int valores[5];
    int indice = 0;
    int inicio = 0;
    int fin = inputString.indexOf(',');

    while (fin != -1 && indice < 5) {
      valores[indice] = inputString.substring(inicio, fin).toInt();
      inicio = fin + 1;
      fin = inputString.indexOf(',', inicio);
      indice++;
    }
    if(indice < 5) {
      valores[indice] = inputString.substring(inicio).toInt();
    }

    // --- MOVER LOS SERVOS ---
    for(int i=0; i<5; i++){
      // Filtro de seguridad 0-180
      if(valores[i] > 180) valores[i] = 180;
      if(valores[i] < 0) valores[i] = 0;
      
      servos[i].write(valores[i]);
      actual_position[i] = valores[i];
      delay(10); ////////////////////////////////////////////////////////////////////////////////
    }

    // Limpiar
    inputString = "";
    stringComplete = false;
  }
  
}


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    // Si llega salto de línea, terminamos
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}*/
