#include <cstdlib>
#include <iostream>
#include <poll.h>

#include "OptimusPiConfig.h"
#include "PiWars.h"
#include "InputDevice.h"
#include "InputEvent.h"
#include "Powertrain.h"
#include "ThoughtProcess_Manual.h"

int main(int argc,char *argv[]) {
  std::cout << "Hello World!" << std::endl;
  std::cout << "Version " << OptimusPi_VERSION_MAJOR << "." << OptimusPi_VERSION_MINOR << std::endl;  
  PiWars::PiWars optimusPi;
  PiWars::ThoughtProcess *manual = new PiWars::ThoughtProcess_Manual(&optimusPi);
  
  while(!manual->available()) {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }

  if(manual->prepare()) {
    manual->run();
  }
  
  return EXIT_SUCCESS;
}

