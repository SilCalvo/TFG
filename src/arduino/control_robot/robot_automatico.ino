// ================================================================
// --- AUTOMATIC CONTROL MODE ---
// ================================================================
/*
 * Receives angle commands via Serial communication (from ROS2).
 * Parses the incoming integer stream, applies safety limits (0-180), and updates the servo positions accordingly.
 */

void automatic_control(){

  int target_position[NUMBER_SERVOS]= {0,0,0,0};
  if (Serial.available() > 0) {
    for (int i = 0; i < NUMBER_SERVOS; i++) {
      int lectura = Serial.parseInt();
      target_position[i] = lectura;
      
      // Security limits
      if(target_position[i] > 180) target_position[i] = 180;
      if(target_position[i] < 0) target_position[i] = 0;
    }
    
    // Clean Serial
    while(Serial.available()) { Serial.read(); }

    // Move servos
    for(int i=0; i<NUMBER_SERVOS; i++){
      servos[i].write(target_position[i]);
      actual_position[i] = target_position[i];
    }

  }
}