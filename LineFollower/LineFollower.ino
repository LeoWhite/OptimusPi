#include "LineFollower.h"
#include "QTRSensors.h"
#include <avr/wdt.h>

/**
 * Sets up the Arduino ready for use
 */
void setup() {
  // Reset the watchdog in case it triggered
  MCUSR=0;
  wdt_disable();
  
  // start serial for output (Used for debugging)
  Serial.begin(115200);    

  // Configure sensor
  qtr8RCSetup();

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
  // Check if there are any pending i2c commands to process
  I2C_CheckCommands();

  // Process any sensor information
  qtr8RCLoop();
  
  // Reset the watchdog to stop it triggering
  wdt_reset();
}

