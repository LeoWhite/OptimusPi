#include "MotorDriver.h"
#include "DualVNH5019MotorShield.h"
#include <avr/wdt.h>

Motor motors[2]; //<! Array to hold details on the 2 motors this system has

DualVNH5019MotorShield md; //<! The object for accessing the motor shield

unsigned long lastOverloadMS = 0; //<! Time we last detected an overloaded

/**
 * Sets up the Arduino ready for use
 */
void setup() {
  // Reset the watchdog in case it triggered
  MCUSR=0;
  wdt_disable();

  // start serial for output (Used for debugging)
  Serial.begin(115200);

  // Configure motor
  motorsSetup();

  // Configure motor encoders
  // TODO: Currently no encoders supported
  //encodersSetup();

  // initialize i2c
  I2CSetup();

  // With setup complete we now enable the watchdog
  wdt_enable(WDTO_250MS);

  Serial.println("Setup complete");
}

/**
 * main loop
 */
void loop() {
  // Read in the current amps
  motors[LEFT_MOTOR].current = md.getM1CurrentMilliamps();
  motors[RIGHT_MOTOR].current = md.getM2CurrentMilliamps();

  // Check if we've gone over the limit
  if(motors[LEFT_MOTOR].current >= CURRENT_OVERLOAD_CUTOUT || motors[RIGHT_MOTOR].current >= CURRENT_OVERLOAD_CUTOUT) {
    // Mark the fact that we have overloaded and trigger
    // an update of the Motors, this will cause them to stop
    lastOverloadMS = millis();
    Motors(0, 0);
  }

  // Check if there are any pending i2c commands to process
  I2C_CheckCommands();

  // Reset the watchdog to stop it triggering
  wdt_reset();
}

