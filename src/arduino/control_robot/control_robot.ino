// ================================================================
// --- MAIN CONTROL LOOP (MODES & HARDWARE SETUP) ---
// ================================================================
/*
 * Manages the switching between Automatic (Serial/ROS) and Manual (Potentiometers) modes
 * using physical buttons with debounce. Updates status LEDs to indicate the active mode.
*/

#include <Servo.h>
#include <ezButton.h> //INSTALAR

#define NUMBER_SERVOS 4

// ROBOT SERVOS PINS
int BASE_SERVO = 3;
int SHOULDER_SERVO = 4;
int ELBOW_SERVO = 5;
int END_EFFECTOR_SERVO = 6;

// ROBOT POTENCIOMETERS PINS
int BASE_POTENCIOMETER = A0;
int SHOULDER_POTENCIOMETER = A1;
int ELBOW_POTENCIOMETER = A2;
int END_EFFECTOR_POTENCIOMETER = A3;

// ROBOT MODE PINS
int PIN_AUTOMATIC = 9;
int PIN_MANUAL = 10;
int PIN_WALL = 12;
int LED_AUTOMATIC = 8;
int LED_MANUAL = 11;
int LED_WALL = 13;

// ROBOT MODE FLAGS
bool automatic_mode = false;
bool manual_mode = false;
bool wall = false;


// ROBOT CONTROL ARRAYS
Servo servos[NUMBER_SERVOS];
int ROBOT_PINS[NUMBER_SERVOS] = {BASE_SERVO, SHOULDER_SERVO, ELBOW_SERVO, END_EFFECTOR_SERVO};
int ROBOT_POTENCIOMETERS[NUMBER_SERVOS] = {BASE_POTENCIOMETER, SHOULDER_POTENCIOMETER, ELBOW_POTENCIOMETER, END_EFFECTOR_POTENCIOMETER};
int actual_position[NUMBER_SERVOS]= {90,90,90,135};

ezButton automatic_button(PIN_AUTOMATIC);
ezButton manual_button(PIN_MANUAL);
ezButton wall_button(PIN_WALL);

void setup() {
  Serial.begin(57600);
  Serial.setTimeout(10);

  // BUTTONS MODE INIT
  automatic_button.setDebounceTime(75);
  manual_button.setDebounceTime(75);
  wall_button.setDebounceTime(75);

  pinMode(LED_AUTOMATIC, OUTPUT);
  pinMode(LED_MANUAL, OUTPUT);
  pinMode(LED_WALL, OUTPUT);

  // SERVOS INIT 
  for(int i=0; i<NUMBER_SERVOS; i++){
    servos[i].attach(ROBOT_PINS[i]);
    servos[i].write(actual_position[i]); 
  }
}

void loop() {

  // CHECK MODE CHANGE 
  automatic_button.loop();
  manual_button.loop();
  wall_button.loop();

  if(automatic_button.isPressed()) {
    automatic_mode = true;
    manual_mode = false;
    digitalWrite(LED_AUTOMATIC, HIGH);
    smooth_manual_control();
    digitalWrite(LED_MANUAL, LOW);
    Serial.println("AUTO"); 
  }

  if(manual_button.isPressed()) {
    automatic_mode = false;
    manual_mode = true;
    digitalWrite(LED_AUTOMATIC, LOW);
    digitalWrite(LED_MANUAL, HIGH);
    Serial.println("MANUAL"); 
  }

  if (wall_button.isPressed() && manual_mode) {
    wall = true;
    digitalWrite(LED_WALL, HIGH);
  }

  // EXECUTE CONTROL 
  if (automatic_mode) {automatic_control();} // AUTOMATIC MODE CONTROL
  if (manual_mode) {manual_control();}  // MANUAL MODE CONTROL

  delay(10);

}
