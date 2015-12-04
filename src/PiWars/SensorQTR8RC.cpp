/**
 * The SensorQTR8RC class initialises and controls access to
 * the SparkFun VL6180 Range sensor.
 * https://cdn.sparkfun.com/assets/learn_tutorials/2/5/7/VL6180_ApplicationNote.pdf
 */

#include "SensorQTR8RC.h"

#include <iostream>
#include <thread>

namespace PiWars
{


SensorQTR8RC::SensorQTR8RC() : Sensor(), I2CExternal(0x8), _initialised(false) {
}

SensorQTR8RC::~SensorQTR8RC() {
}

bool SensorQTR8RC::exists() {
  return I2C::exists();
}

bool SensorQTR8RC::enable() {
  if(!isEnabled()) {
    uint8_t byte;

    // Perform calibration here

    if(writeByte('\x11')) {
      std::cout << "Calibration started" << std::endl;

      // Allow 5 seconds for calibration
      //std::this_thread::sleep_for (std::chrono::seconds(5));

      if(writeByte('\x12')) {
        std::cout << "Calibration complete" << std::endl;
      }
    }

    // Call the base class to perform any
    // generic changes
    Sensor::enable();
  }

  return isEnabled();

}

void SensorQTR8RC::disable() {
  if(isEnabled()) {
    Sensor::disable();
  }
}

bool SensorQTR8RC::readLine(uint16_t (&sensorDiff)[8], uint16_t &position) {
  bool success = false;

  // Attempt to read from the sensor
  if(writeByte('\x13')) {
    char i2cResponse[32];
    size_t read, i2cResponseArg = 0;

    // Allow the Arduino time to take a reading
    std::this_thread::sleep_for (std::chrono::milliseconds(5));

    // and read in the results
    read = readBytes(i2cResponse, 19);

    // Check its a valid result
    if(19 == read && 0x13 == i2cResponse[i2cResponseArg++]) {
      // and process the results
      for(size_t i = 0; i < 8; i++) {
        sensorDiff[i] = ((i2cResponse[i2cResponseArg++] << 8) | i2cResponse[i2cResponseArg++]);
      }

      position = (i2cResponse[i2cResponseArg++] << 8) | i2cResponse[i2cResponseArg++];

      success = true;
    }
    else {
      std::cerr << __func__ << "Failed to read in data from i2c. Got " << read << std::endl;
    }

  }
  return success;
}

}
