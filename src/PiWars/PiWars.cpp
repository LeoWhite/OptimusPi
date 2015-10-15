#include "PiWars.h"
#include "Brains.h"
#include "Powertrain.h"

namespace PiWars
{

  PiWars::PiWars() : _brains(new Brains()), _powertrain(new Powertrain())
  {

  }
  
  PiWars::~PiWars() {
    delete _brains;
    delete _powertrain;
  }
}
