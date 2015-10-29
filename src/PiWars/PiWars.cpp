#include "PiWars.h"
#include "Brains.h"
#include "Powertrain.h"

namespace PiWars
{

  PiWars::PiWars() : _brains(new Brains()), _powertrain(new Powertrain())
  {
  // Stop the motors
  _powertrain->stop();
  }
  
  PiWars::~PiWars() {
    delete _brains;
    delete _powertrain;
  }
}
