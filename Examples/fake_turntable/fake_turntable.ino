/* Example sketch to control a 28BYJ-48 stepper motor with ULN2003 driver board, AccelStepper and Arduino UNO: continuous rotation. More info: https://www.makerguides.com */
// Include the AccelStepper library:
#include <AccelStepper.h>
// Motor pin definitions:
#define motorPin1  8      // IN1 on the ULN2003 driver
#define motorPin2  9      // IN2 on the ULN2003 driver
#define motorPin3  10     // IN3 on the ULN2003 driver
#define motorPin4  11     // IN4 on the ULN2003 driver
// Define the AccelStepper interface type; 4 wire motor in half step mode:
#define MotorInterfaceType 8
// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper library with 28BYJ-48 stepper motor:
AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);

// Define Pins
int interruptPin = 2;
int val = 0;     // variable to store the read value
int switch1 = 0;
int state = 0;

//Set spinning speed
int clockSpeed = 1000;
int counterSpeed = -1000;

void setup() {
  // Set the maximum steps per second:
  stepper.setMaxSpeed(2500);

  // Pin Mode
  pinMode(inPin1, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR, RISING);

  // Set up Serial
  Serial.begin(9600);
}
void loop() {
  if (state = 0) {
//    Serial.println("Spin Motor Clockwise");
    // Set the speed of the motor in steps per second:
    stepper.setSpeed(clockSpeed);
  } 
  else if (state = 1) {
//    Serial.println("Spin Motor Counter Clockwise");
    // Set the speed of the motor in steps per second:
    stepper.setSpeed(counterSpeed);
  }
  else if (state = 2) {
//    Serial.println("Stop Spinning motor");
    stepper.setSpeed(0);
  }

  // Step the motor with constant speed as set by setSpeed():
  stepper.runSpeed();
}

void ISR() {
  switch (state) { 
    case 0:
      state = 1;
    case 1:
      state = 2:
    case 2:
      state = 0;
    default:
      state = 2;
  }
}
