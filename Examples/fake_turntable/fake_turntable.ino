/* Example sketch to control a 28BYJ-48 stepper motor with ULN2003 driver board, AccelStepper and Arduino UNO: continuous rotation. More info: https://www.makerguides.com */
// Include the AccelStepper library:
#include <AccelStepper.h>
// Motor pin definitions:
#define motorPin1  8      // IN1 on the ULN2003 driver
#define motorPin2  9      // IN2 on the ULN2003 driver
#define motorPin3  10     // IN3 on the ULN2003 driver
#define motorPin4  11     // IN4 on the ULN2003 driver
// Define the AccelStepper interface type; 4 wire motor in half step mode:
#define MotorInterfaceType 4
// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper library with 28BYJ-48 stepper motor:
AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);

// Define Pins
int analogPin = 1;
int interruptPin = 2;
int val = 0;     // variable to store the read value
int switch1 = 0;
int state = 0;

//Set spinning speed
int clockSpeed = 1000;
float pos = 0; //current stepper position

// use 2ms debounce time
#define DEBOUNCE_TICKS (word)microsecondsToClockCycles(15)

extern volatile unsigned long timer0_overflow_count;
word keytick;  // record time of keypress

void setup() {
  // Set the maximum steps per second:
  stepper.setMaxSpeed(1000);

  // Pin Mode
  attachInterrupt(digitalPinToInterrupt(interruptPin),KeyPress, FALLING);

  // Set up Serial
  Serial.begin(115200);
}

void loop() {
  val = analogRead(analogPin);
  //Debounce check
  if (KeyCheck()) {
    state = CHANGE_STATE(state);
  }
  if (state == 0) { //Motor stops
    stepper.setSpeed(0);
    stepper.runSpeed();
  } 
  else if (state == 1) { //Motor follows pot
    stepper.setSpeed(10);
    stepper.runSpeed();
    pos = stepper.currentPosition();
    Serial.print("Current Position: ");
    Serial.println(pos);
  }
  else if (state == 2) { //Motor turns at constant speed
    stepper.setSpeed(clockSpeed);
    stepper.runSpeed();
  }
}

int CHANGE_STATE(int cur_state) {
  int new_state = 0;
  Serial.print("Current State: ");
  Serial.println(cur_state);
  if (cur_state == 0) {
    new_state = 1;
  } 
  else if (cur_state == 1) {
    new_state = 2;
  }
  else if (cur_state == 2) {
    new_state = 0;
  }
  else {
    state = 0;
  }
  
  
  Serial.print("New State: ");
  Serial.println(new_state);
  return new_state;
}

void KeyPress() {
  keytick=(word)timer0_overflow_count;
}

// returns true if key pressed
boolean KeyCheck() {
  if (keytick!=0) {
    if (((word)timer0_overflow_count-keytick)>DEBOUNCE_TICKS) {
      keytick=0;
      return true;
    }
  }
  return false;
}
