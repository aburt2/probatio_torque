**
   @brief TorqueTuner-uStepperS example sketch, adapted from Mathias Kirkegaard's TorqueTuner for Mechaduino
  @author Christian Frisson
  @date 2021-07-19
*/

#include <uStepperS.h>

#include <Wire.h>
#include <math.h>

uStepperS stepper;
char mode;
float r = 0;  //setpoint

//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

const uint8_t I2C_BUF_SIZE = 10;
const uint8_t CHECKSUMSIZE = 2;

uint8_t rx_data[I2C_BUF_SIZE];
uint8_t tx_data[I2C_BUF_SIZE];

uint16_t checksum_tx = 0;
uint16_t checksum_rx = 0;
uint16_t sum = 0;

float tmp;
int16_t angle_rounded = 0;
char mode_rec = 't';
float velocity = 0;
int16_t torque = 0;
int32_t err = 0;

uint16_t calcsum(uint8_t buf[], int length) {
  uint32_t val = 0;
  for (int k = 0; k < length; k++) {
    val += buf[k];
  }
  return val;
}

void receiveI2C(int how_many) {
  uint8_t k = 0;
  while (Wire.available()) {
    rx_data[k] = Wire.read();
    k++;
    if (k > I2C_BUF_SIZE + CHECKSUMSIZE) break;
  }
  memcpy(&checksum_rx, rx_data + I2C_BUF_SIZE, 2);

  if (checksum_rx != calcsum(rx_data, I2C_BUF_SIZE)) { // error in recieved data
    err++;
  } else {
    memcpy(&torque, rx_data, 2); // int16
    memcpy(&velocity, rx_data + 2, 4); // float
    memcpy(&mode_rec, rx_data + 6, 1); // char
    if (mode_rec == 't' || mode_rec == 'x' || mode_rec == 'v') {
      mode = mode_rec;
    };
  }

}

void sendI2C() {
  // Angle
  tmp = stepper.encoder.getAngle();
  angle_rounded = static_cast<int16_t> (tmp * 10 + 0.5);
  // angle_rounded = static_cast<int16_t> (read_angle() * 10 + 0.5);
  memcpy(tx_data, &angle_rounded, 2);

  // wrapped angle_rounded
  tmp = stepper.encoder.getAngleMoved();
  angle_rounded = static_cast<int16_t>(tmp * 10 + 0.5);
  memcpy(tx_data + 2, &angle_rounded, 2); // uint

  // Velocity
  tmp = stepper.encoder.getSpeed(); // Current speed in 1/256th steps per second
  memcpy(tx_data + 4, &tmp, 4); // float

  // Checksum
  checksum_tx = calcsum(tx_data, I2C_BUF_SIZE);
  memcpy(tx_data + I2C_BUF_SIZE, &checksum_tx, 2); // uint16

  // Send tx_data to I2C master
  Wire.write(tx_data, I2C_BUF_SIZE + CHECKSUMSIZE);
}

void setup()
{
  Serial.begin(115200);
  stepper.setup();
  stepper.checkOrientation(30.0); // Check orientation of motor connector with
  // +/- 30 microsteps movement

  stepper.disableStallguard();

  mode = 't';                   // start in torque mode

  // // I2C
  Wire.begin(8);
  Wire.onReceive(receiveI2C);
  Wire.onRequest(sendI2C);
}

//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////


void loop()
{

  // Set torque or velocity
  switch (mode) {
  case 't':
    r = torque;
    stepper.setBrakeMode(HARDBRAKE, 100.0 * torque);
    break;
  case 'v':
    if (abs(r - velocity) > 1) r = velocity;
    stepper.setBrakeMode(FREEWHEELBRAKE);
    stepper.setRPM(r);
    break;
  }

  Serial.print(" Angle ");
  Serial.println(stepper.encoder.getAngleMoved());

}
