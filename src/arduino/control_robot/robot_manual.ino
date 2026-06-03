// ================================================================
// --- MANUAL MODE ---
// ================================================================
/*
 * Reads analog values from the potentiometers and maps them to servo angles (0-180).
 * Updates the servos only if the change is significant (>= 1 degree) to prevent jitter/vibration caused by signal noise.
 * Sends the current joint angles via Serial to ROS 2 (Format: "j1,j2,j3,j4,j5\n") so the PC knows the robot's pose.
 */

unsigned long last_manual_update = millis();
const int MANUAL_INTERVAL = 200;
int point_step = 0;

void execute_calibration_points() {
  
  int points[6][4] = {
    // --- PARED 1: TECHO ---
    {90,  120, 60,  90},  // P1: Frente
    {135, 120, 60,  90},  // P2: Izquierda
    {45,  120, 60,  90},  // P3: Derecha
    
    // --- PARED 2: LATERAL INCLINADA ---
    {135, 90,  90,  90},  // P4: Arriba diagonal
    {180, 90,  90,  90},  // P5: Arriba izquierda
    {157, 135, 135, 90}   // P6: Abajo centro
  };

  if (point_step < 6) {
    smooth_manual_control(points[point_step]);

    point_step++;
    
  } else {
    point_step = 0; 
  }
}

int parse_potenciometer(int pin) {
  int read = analogRead(pin);
  int angle = map(read, 0, 1023, 0, 180);
  angle = constrain(angle, 0, 180);
  
  return angle;
}
void smooth_manual_control(int target_angles[]) { 

  for (int i = 0; i < NUMBER_SERVOS; i++) {

    while (abs(target_angles[i] - actual_position[i]) > 5) {

      if (target_angles[i] > actual_position[i]) {
        actual_position[i] += 5;
      }
      else {
        actual_position[i] -= 5;
      }
      
      servos[i].write(actual_position[i]); 
      delay(20); 
    }
    
    actual_position[i] = target_angles[i];
    servos[i].write(actual_position[i]); 
  }
}

void manual_control(){

  if (millis() - last_manual_update < MANUAL_INTERVAL) {
    return;
  }
  last_manual_update = millis();

  // Control servos
  for (int i = 0; i < NUMBER_SERVOS; i++) {
    int angle = parse_potenciometer(ROBOT_POTENCIOMETERS[i]);
    if (abs(angle - actual_position[i]) >=1) {
      servos[i].write(angle); 
      actual_position[i]=angle;
    }
  }

  if(wall == true) {
    Serial.print("WALL");
    //execute_calibration_points();
    wall= false;
  }
  Serial.print("(");

  // Send servos angles via Serial
  for (int i = 0; i < NUMBER_SERVOS; i++) {
    Serial.print(actual_position[i]);
    // Add a comma after the number
    if (i < NUMBER_SERVOS -1 ) {
      Serial.print(",");
    }
  }
  Serial.print(")");
  
  
  // End the message with a New Line so ROS2 knows the packet is finished
  Serial.println(); 
  digitalWrite(LED_WALL, LOW);

}
