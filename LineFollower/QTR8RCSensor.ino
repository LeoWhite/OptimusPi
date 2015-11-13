// This is setup for the Pololu QTR-8RC Reflectance Sensor Array
// Library is available from https://github.com/pololu/qtr-sensors-arduino

unsigned int sensorValues[qtr8RCSensorCount]; //<! Where the sensor values are stored
bool calibrating = false; //<! Flag to indicate if we are currently calibrating

// The class for accessing the sensor
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
 * Process the i2c calibrate start command, resetting any
 * existing calibration information.
 *
 * @param i2cArgs Any arguments passed on i2c
 * @param pi2cResponse Filled in with the I2C response, if any
 *
 * @returns the number of items added to the response
 */
int qtr8RCCalibrateStart(byte *i2cArgs, uint8_t *pi2cResponse) {
  // reset the calibration information
  qtrrc.resetCalibration();

  calibrating = true;
  return 0;
}

/**
 * Process the i2c calibrate stop command, stopping the
 * calibration process and outputting the values over Serial
 *
 * @param i2cArgs Any arguments passed on i2c
 * @param pi2cResponse Filled in with the I2C response, if any
 *
 * @returns the number of items added to the response
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
 * Process the i2c read sensor command.
 * Stops any calibration currently in progress and reads in the
 * current results from the sensor array
 *
 * @param i2cArgs Any arguments passed on i2c
 * @param pi2cResponse Filled in with the I2C response, if any
 *
 * @returns the number of items added to the response
 */
int qtr8RCReadSensor(byte *i2cArgs, uint8_t *pi2cResponse) {
  unsigned int position = qtrrc.readLine(sensorValues);
  uint8_t i2cResponseArg = 0;

  if(calibrating) {
    qtr8RCCalibrateStop(i2cArgs, pi2cResponse);

  }

  // Convert results to the i2c format
  for (unsigned char i = 0; i < qtr8RCSensorCount; i++) {
    // Each result is stored as a 16 bit value
    pi2cResponse[i2cResponseArg++] = highByte(sensorValues[i]);
    pi2cResponse[i2cResponseArg++] = lowByte(sensorValues[i]);
  }

  // Store the position as a 16 bit value
  pi2cResponse[i2cResponseArg++] = highByte(position);
  pi2cResponse[i2cResponseArg++] = lowByte(position);

  return i2cResponseArg;
}
