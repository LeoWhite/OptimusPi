// This is setup for the Pololu Dual VNH5019 Motor Driver Shield
// Library is available from https://github.com/pololu/dual-vnh5019-motor-shield

// How long to disable the motors after an overload
// has been detected
#define OVERLOAD_COOLDOWN_MS 100

/**
 * Configures the Arduino's pins that will be used to control the motors
 */
void motorsSetup() {
  md.init();

  // Initialise the motor arrays
  memset(motors, 0, sizeof(motors));

  // Ensure the motors are not running
  MotorsStop();
}

/**
 * Sets the speed of the motors.
 * The motor power is convereted into a percentage so this
 * routine takes in values from -100 to 100, with the sign of
 * indicating the direction (i.e. negative numbers will
 * make the motor go backwards)
 *
 * @param left - Speed of the left motor
 * @param right - Speed of the right motor
 */
void Motors(int left, int right)
{
  int lmspeed, rmspeed;

  // Are we in an 'overload' state?
  if((millis() - lastOverloadMS) < OVERLOAD_COOLDOWN_MS) {
    // Set speed to zero and brakes to on
    lmspeed = 0;
    motors[LEFT_MOTOR].brake=true;
    rmspeed = 0;
    motors[RIGHT_MOTOR].brake=true;
  }

  // Are the inputs valid
  if(left > 100 || left < -100 || right > 100 || right < -100) {
    Serial.println("Motor input invalid, ignoring");
    return;
  }

  // Store the percentage values
  motors[LEFT_MOTOR].power = left;
  motors[RIGHT_MOTOR].power = right;

  // Convert from percentage to actual value
  // The library uses a range of -400 to 400
  lmspeed = (motors[LEFT_MOTOR].power * 400) / 100;
  rmspeed = (motors[RIGHT_MOTOR].power * 400) /100;

  // Are we braking?
  if(motors[LEFT_MOTOR].brake) {
    md.setM2Brake(200);
  }
  else {
    md.setM2Speed(-lmspeed);
  }

  if(motors[LEFT_MOTOR].brake>0 && motors[LEFT_MOTOR].power==0) {
    // if left brake is enabled and left speed=0 then reset left encoder counter
    motors[LEFT_MOTOR].encoderCount=0;
  }

  // Are we braking?
  if(motors[RIGHT_MOTOR].brake) {
    md.setM1Brake(200);
  }
  else {
    md.setM1Speed(rmspeed);
  }

  if(motors[RIGHT_MOTOR].brake>0 && motors[RIGHT_MOTOR].power==0) {
    // if left brake is enabled and left speed=0 then reset left encoder counter
    motors[RIGHT_MOTOR].encoderCount=0;
  }

  Serial.print("Motors =");
  Serial.print(lmspeed);
  Serial.print(":");
  Serial.println(rmspeed);
}

/**
 * Stops the motors and enables the brakes
 */
void MotorsStop() {
  // Enable brakes
  motors[LEFT_MOTOR].brake=true;
  motors[RIGHT_MOTOR].brake=true;

  Motors(0, 0);
}

/**
 * Process the i2c stop command
 *
 * @param i2cArgs Any arguments passed on i2c
 * @param pi2cResponse Filled in with the I2C response, if any
 *
 * @returns the number of items added to the response
 */
int motorsI2CStop(byte *i2cArgs, uint8_t *pi2cResponse) {
  // No arguments to process, so just stop the motors
  MotorsStop();

  return 0;
}

/**
 * Process the i2c set motors command
 *
 * Takes in the two integers that contain the power levels
 * of the left and right motors
 *
 * @param i2cArgs Any arguments passed on i2c
 * @param pi2cResponse Filled in with the I2C response, if any
 *
 * @returns the number of items added to the response
 */
int motorsI2CSet(byte *i2cArgs, uint8_t *pi2cResponse) {
  int left, right;
  boolean gotLeft = false, gotRight = false;

  // read integer from I2C buffer
  left=(i2cArgs[0] * 256) + i2cArgs[1];
  if(left >= -100 && left <= 100) {
    motors[LEFT_MOTOR].brake=false;
    gotLeft = true;
  }

  // read integer from I2C buffer
  right=(i2cArgs[2] * 256) + i2cArgs[3];
  if(right >= -100 && right <= 100) {
    motors[RIGHT_MOTOR].brake=false;
    gotRight = true;
  }

  if(gotLeft && gotRight) {
    Motors(left, right);
  }

  return 0;
}
