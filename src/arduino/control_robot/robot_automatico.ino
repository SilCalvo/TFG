// ================================================================
// --- AUTOMATIC CONTROL MODE ---
// ================================================================
/*
 * Receives angle commands via Serial communication (from ROS2).
 * Parses the incoming integer stream, applies safety limits (0-180), and updates the servo positions accordingly.
 */

void automatic_control(){

  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n'); 
    data.trim(); 
    
    if (data.startsWith("D{") && data.endsWith("}")) {
      
      String numbers = data.substring(2, data.length() - 1);
      
      int target_position[NUMBER_SERVOS]= {0,0,0,0};
      int startIdx = 0;
      
      for (int i = 0; i < NUMBER_SERVOS; i++) {
        int commaIdx = numbers.indexOf(',', startIdx);
        String valStr = "";
        
        if (commaIdx != -1) {
          valStr = numbers.substring(startIdx, commaIdx);
          startIdx = commaIdx + 1;
        } else {
          valStr = numbers.substring(startIdx);
        }
        
        int lectura = valStr.toInt();
        target_position[i] = lectura;
        
        // Security limits
        if(target_position[i] > 180) target_position[i] = 180;
        if(target_position[i] < 0) target_position[i] = 0;
      }
      
      // Move servos
      for(int i=0; i<NUMBER_SERVOS; i++){
        servos[i].write(target_position[i]);
        actual_position[i] = target_position[i];
      }
    }
    
    // Clean Serial
    while(Serial.available()) { Serial.read();
    }

  }
}