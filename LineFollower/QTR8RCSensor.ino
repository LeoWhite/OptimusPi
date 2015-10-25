// This is setup for the Pololu QTR-8RC Reflectance Sensor Array
// Library is available from https://github.com/pololu/qtr-sensors-arduino

unsigned int sensorValues[qtr8RCSensorCount];

bool calibrating = false;

QTRSensorsRC qtrrc((unsigned char[]) {qtr8RCSensorOne, qtr8RCSensorTwo, qtr8RCSensorThree, qtr8RCSensorFour, qtr8RCSensorFive, qtr8RCSensorSix,qtr8RCSensorSeven, qtr8RCSensorEight}, qtr8RCSensorCount, qtr8RCTimeout, qtr8RCEmitter);


/**
 * Configures the QTR8 Sensor
 */
void qtr8RCSetup() {
  // No additional setup currently required
}

/**
 * Called during the main 'loop' cycle to perform any
 * in progress operations
 */
void qtr8RCLoop() {
  // If we are calibrating then update the sensors
  if(calibrating) {
    qtrrc.calibrate();
  }
}

/**
 * Process the i2c calibrate start command
 */
int qtr8RCCalibrateStart(byte *i2cArgs, uint8_t *pi2cResponse) {
  // reset the calibration information
  qtrrc.resetCalibration();
  
  calibrating = true;
  return 0;
}

/**
 * Process the i2c calibrate stop command
 */
int qtr8RCCalibrateStop(byte *i2cArgs, uint8_t *pi2cResponse) {
  calibrating = false;

  for (int i = 0; i < qtr8RCSensorCount; i++)
  {
    Serial.print(qtrrc.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < qtr8RCSensorCount; i++)
  {
    Serial.print(qtrrc.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  
  return 0;
}

/**
 * Process the i2c read sensor command
 *
 * Takes in the two integers that contain the power levels
 * of the left and right motors
 */
int qtr8RCReadSensor(byte *i2cArgs, uint8_t *pi2cResponse) {
  unsigned int position = qtrrc.readLine(sensorValues);
  uint8_t i2cResponseArg = 0;

  if(calibrating) {
    qtr8RCCalibrateStop(i2cArgs, pi2cResponse);
  
  }
  // Convert results to the i2c format

  // print the sensor values as numbers from 0 to 1000, where 0 means maximum reflectance and
  // 1000 means minimum reflectance, followed by the line position
  for (unsigned char i = 0; i < qtr8RCSensorCount; i++)
  {
    pi2cResponse[i2cResponseArg++] = highByte(sensorValues[i]);
    pi2cResponse[i2cResponseArg++] = lowByte(sensorValues[i]);
    //Serial.print(sensorValues[i]);
    //Serial.print('\t');
  }

  // Store the position as a 16 bit value
  pi2cResponse[i2cResponseArg++] = highByte(position);
  pi2cResponse[i2cResponseArg++] = lowByte(position);
  
  //Serial.println(); // uncomment this line if you are using raw values
  //Serial.println(position); // comment this line out if you are using raw values

  return i2cResponseArg;
}
