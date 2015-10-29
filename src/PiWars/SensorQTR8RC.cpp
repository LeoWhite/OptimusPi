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
       
    // We should perform calibration here...

  if(writeByte('\x11')) {
    std::cerr << "Calibraoin started" << std::endl;
    std::this_thread::sleep_for (std::chrono::seconds(5));
      
                  
    if(writeByte('\x12')) {
      std::cerr << "Calibraoin complete" << std::endl;
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
    
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
    
    read = readBytes(i2cResponse, 19);
    
    if(19 == read && 0x13 == i2cResponse[i2cResponseArg++]) {
      for(size_t i = 0; i < 8; i++) {
        sensorDiff[i] = ((i2cResponse[i2cResponseArg++] << 8) | i2cResponse[i2cResponseArg++]);
      }
      
      position = (i2cResponse[i2cResponseArg++] << 8) | i2cResponse[i2cResponseArg++];
      
      success = true;
        
    }
    else {
      std::cerr << "Failed to read in data from i2c. Got " << read << std::endl;
    }

  }
  return success;
}
}
