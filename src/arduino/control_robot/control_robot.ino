// ================================================================
// --- MAIN CONTROL LOOP (MODES & HARDWARE SETUP) ---
// ================================================================
/*
 * Manages the switching between Automatic (Serial/ROS) and Manual (Potentiometers) modes
 * using physical buttons with debounce. Updates status LEDs to indicate the active mode.
*/

#include <Servo.h>
#include <ezButton.h> //INSTALAR

// ROBOT SERVOS PINS
int BASE_SERVO = 3;
int SHOULDER_SERVO = 4;
int ELBOW_SERVO = 5;
int WRIST_SERVO = 6;
int END_EFFECTOR_SERVO = 7;

// ROBOT POTENCIOMETERS PINS
int BASE_POTENCIOMETER = A0;
int SHOULDER_POTENCIOMETER = A1;
int ELBOW_POTENCIOMETER = A2;
int WRIST_POTENCIOMETER = A3;
int END_EFFECTOR_POTENCIOMETER = A4;

// ROBOT MODE PINS
int PIN_AUTOMATIC = 9;
int PIN_MANUAL = 10;
int LED_AUTOMATIC = 8;
int LED_MANUAL = 11;

// ROBOT MODE FLAGS
bool automatic_mode = false;
bool manual_mode = false;

// ROBOT CONTROL ARRAYS
Servo servos[5];
int ROBOT_PINS[5] = {BASE_SERVO, SHOULDER_SERVO, ELBOW_SERVO, WRIST_SERVO, END_EFFECTOR_SERVO};
int ROBOT_POTENCIOMETERS[5] = {BASE_POTENCIOMETER, SHOULDER_POTENCIOMETER, ELBOW_POTENCIOMETER, WRIST_POTENCIOMETER, END_EFFECTOR_POTENCIOMETER};
int actual_position[5]= {90,90,90,90,90};

ezButton automatic_button(PIN_AUTOMATIC);
ezButton manual_button(PIN_MANUAL);

void setup() {
  Serial.begin(57600);
  Serial.setTimeout(10);

  // BUTTONS MODE INIT
  automatic_button.setDebounceTime(50);
  manual_button.setDebounceTime(50);

  pinMode(LED_AUTOMATIC, OUTPUT);
  pinMode(LED_MANUAL, OUTPUT);

  // SERVOS INIT -------------------------------------------- QUITAR --------------------------------------------
  for(int i=0; i<5; i++){
    servos[i].attach(ROBOT_PINS[i]);
    servos[i].write(90); 
    actual_position[i]=90;
  }
}

void loop() {

  // CHECK MODE CHANGE 
  automatic_button.loop();
  manual_button.loop();

  if(automatic_button.isPressed()) {
    automatic_mode = true;
    manual_mode = false;
    digitalWrite(LED_AUTOMATIC, HIGH);
    digitalWrite(LED_MANUAL, LOW);
  }

  if(manual_button.isPressed()) {
    automatic_mode = false;
    manual_mode = true;
    digitalWrite(LED_AUTOMATIC, LOW);
    digitalWrite(LED_MANUAL, HIGH);
  }

  // EXECUTE CONTROL 
  if (automatic_mode) {automatic_control();} // AUTOMATIC MODE CONTROL
  if (manual_mode) {manual_control();}  // MANUAL MODE CONTROL

  delay(1000);

  Serial.println("manual");
  delay(1000);
  Serial.println("automatic");

  
}
