#include "Powertrain.h"
#include "InputDevice.h"

#include <iostream>

namespace PiWars
{

Powertrain::Powertrain() : I2CExternal(0x07), _powerLeft(0.0f), _powerRight(0.0f), _powerLimiter(1.0f) {
}

Powertrain::~Powertrain() {
  // Stop the motors
  stop();
}

void Powertrain::stop() {
  if(exists()) {
    if(!writeBytes((const char *)"\x11", 1)) {
      std::cerr << "Failed to send stop!" << std::endl;
    }
  }
}

bool Powertrain::setPower(float left, float right) {
  bool result = false;
  
  if(left >= -1.0f && left <= 1.0f && right >= -1.0f && right <= 1.0f) {
    _powerLeft = left;
    _powerRight = right;
    
    if(exists()) {
      char message[5];
      int16_t powerLeft = 100 * _powerLeft * _powerLimiter;
      int16_t powerRight = 100 * _powerRight * _powerLimiter;
      
      // Prepare the message for sending        
      message[0] = '\x12';
      message[1] = (powerLeft >> 8) & 0xFF;
      message[2] = (powerLeft) & 0xFF;
      message[3] = (powerRight >> 8) & 0xFF;
      message[4] = (powerRight) & 0xFF;
  
      if(writeBytes(message, 5) < 0 ) {
        std::cerr << "Failed to send power!" << std::endl;
      }
      else {
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
  
        result = true;
      }
    }
  }
  
  return result;
}

void Powertrain::getPower(float &left, float &right) {
  left = _powerLeft;
  right = _powerRight;
}

bool Powertrain::setLimiter(float limiter) {
  bool result = false;
  
  if(limiter >= 0.0f && limiter <= 1.0f) {
    result = true;
    
    // Has it actually changed?
    if(limiter != _powerLimiter) {
      _powerLimiter = limiter;
      
      // Re-set the power, to cause it to recalculate
      setPower(_powerLeft, _powerRight);
    }
  }
  
  return result;
}  

bool Powertrain::setInputDevice(InputDevice &device, uint32_t leftAxis, uint32_t rightAxis) {
  // TODO: Not currently implemented
  return false;
}

}
