#include "Powertrain.h"
#include "InputDevice.h"

#include <iostream>

namespace PiWars
{

Powertrain::Powertrain() : _powerLeft(0.0f), _powerRight(0.0f), _powerLimiter(1.0f) {
}

Powertrain::~Powertrain() {
}

void Powertrain::stop() {
  // TODO: Implement!
}

bool Powertrain::setPower(float left, float right) {
  bool result = false;
  
  if(left >= -1.0f && left <= 1.0f && right >= -1.0f && right <= 1.0f) {
    _powerLeft = left;
    _powerRight = right;
    
    // TODO: Do something with the values here!
    result = true;
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
      
      // TODO: Recalulate the current power levels
    }
  }
  
  return result;
}  

bool Powertrain::setInputDevice(InputDevice &device, uint32_t leftAxis, uint32_t rightAxis) {
  // TODO: Not currently implemented
  return false;
}

}
