// ================================================================
// --- MANUAL MODE ---
// ================================================================
/*
 * Reads analog values from the potentiometers and maps them to servo angles (0-180).
 * Updates the servos only if the change is significant (>= 1 degree) to prevent jitter/vibration caused by signal noise.
 * Sends the current joint angles via Serial to ROS 2 (Format: "j1,j2,j3,j4,j5\n") so the PC knows the robot's pose.
 */
int parse_potenciometer(int pin) {
  int read = analogRead(pin);
  int angle = map(read, 0, 1023, 0, 180);
  angle = constrain(angle, 0, 180);
  
  return angle;
}

void manual_control(){

  // Control servos
  for (int i = 0; i < 5; i++) {
    int angle = parse_potenciometer(ROBOT_POTENCIOMETERS[i]);
    if (abs(angle - actual_position[i]) >=1) {
      servos[i].write(angle); 
      actual_position[i]=angle;
    }
    delay(10);
  }

  // Send servos angles via Serial
  for (int i = 0; i < 5; i++) {
    Serial.print(actual_position[i]);
    // Add a comma after the number
    if (i < 4) {
      Serial.print(",");
    }
  }
  
  // End the message with a New Line so ROS2 knows the packet is finished
  Serial.println(); 
  delay(100);
}