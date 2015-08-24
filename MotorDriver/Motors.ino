// This is setup for the Moto Monster Shield dual motor driver

// How long to disable the motors after an overload
// has been 
#define OVERLOAD_COOLDOWN_MS 100

/**
 * Configures the Arduino's pins that will be used to control the motors
 */
void motorsSetup() {
  //TCCR2B = TCCR2B & B11111000 | B00000110;   // set timer 2 divisor to  256 for PWM frequency of    122.070312500 Hz
  
  // Setup the left motor
  pinMode(pwm1pin,OUTPUT);
  pinMode(ina1pin,OUTPUT);
  pinMode(inb1pin,OUTPUT);
  
  // Setup the right motor
  pinMode(pwm2pin,OUTPUT);
  pinMode(ina2pin,OUTPUT);
  pinMode(inb2pin,OUTPUT);
  
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
  int lmspeed,rmspeed;
  
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
  lmspeed = (abs(motors[LEFT_MOTOR].power) * 0xFF) / 100;
  rmspeed = (abs(motors[RIGHT_MOTOR].power) * 0xFF) /100;
  
  // Convert to take into account the difference between
  // the input voltage and the target output voltage
  //lmspeed = (lmspeed * powerRatio) / 100;
  //rmspeed = (rmspeed * powerRatio) / 100;
  
  // Are we braking?
  if(motors[LEFT_MOTOR].brake) {
    digitalWrite(ina1pin, LOW);
    digitalWrite(inb1pin, LOW);
  }
  else if(motors[LEFT_MOTOR].power >= 0) {
    // Need to go 'forwards' so set to 'clockwise'
    digitalWrite(ina1pin, HIGH);
    digitalWrite(inb1pin, LOW);    
  }
  // Backwards!
  else {
    digitalWrite(ina1pin, LOW);
    digitalWrite(inb1pin, HIGH);    
  }
  
  analogWrite (pwm1pin,lmspeed);                  // set left PWM to absolute value of left speed - if brake is engaged then PWM controls braking

  if(motors[LEFT_MOTOR].brake>0 && motors[LEFT_MOTOR].power==0) {
    motors[LEFT_MOTOR].encoderCount=0;                  // if left brake is enabled and left speed=0 then reset left encoder counter
  }
  
  // Are we braking?
  if(motors[RIGHT_MOTOR].brake) {
    digitalWrite(ina2pin, LOW);
    digitalWrite(inb2pin, LOW);
  }
  else if(motors[RIGHT_MOTOR].power >= 0) {
    // Need to go 'forwards' so set to 'clockwise'
    digitalWrite(ina2pin, HIGH);
    digitalWrite(inb2pin, LOW);    
  }
  // Backwards!
  else {
    digitalWrite(ina2pin, LOW);
    digitalWrite(inb2pin, HIGH);    
  }

  analogWrite (pwm2pin,rmspeed);                  // set right PWM to absolute value of right speed - if brake is engaged then PWM controls braking
  
  if(motors[RIGHT_MOTOR].brake>0 && motors[RIGHT_MOTOR].power==0) {
    motors[RIGHT_MOTOR].encoderCount=0;                  // if left brake is enabled and left speed=0 then reset left encoder counter
  }  

Serial.print("Motors =");
  Serial.print(motors[LEFT_MOTOR].power);
  Serial.print(":");
  Serial.println(motors[RIGHT_MOTOR].power);
  
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
 */
int motorsI2CSet(byte *i2cArgs, uint8_t *pi2cResponse) {
  int left, right;
  boolean gotLeft = false, gotRight = false;

  // read integer from I²C buffer
  left=i2cArgs[0]*256+i2cArgs[1];                                               
  if(left >= -100 && left <= 100)
  { 
    motors[LEFT_MOTOR].brake=false;
    gotLeft = true;
  }

  // read integer from I²C buffer
  right=i2cArgs[2]*256+i2cArgs[3];
  if(right >= -100 && right <= 100)
  {
    motors[RIGHT_MOTOR].brake=false;
    gotRight = true;
  }

  if(gotLeft && gotRight) {      
    Motors(left, right);
  }
    
  return 0;
}
