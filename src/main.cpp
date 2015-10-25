#include <cstdlib>
#include <iostream>
#include <poll.h>

#include "OptimusPiConfig.h"
#include "PiWars.h"
#include "InputDevice.h"
#include "InputEvent.h"
#include "Powertrain.h"
#include "ThoughtProcess_Manual.h"
#include "ThoughtProcess_Proximity.h"
#include "ThoughtProcess_LineFollower.h"

int main(int argc,char *argv[]) {
  std::cout << "Hello World!" << std::endl;
  std::cout << "Version " << OptimusPi_VERSION_MAJOR << "." << OptimusPi_VERSION_MINOR << std::endl;  
  PiWars::PiWars optimusPi;
  //PiWars::ThoughtProcess *process = new PiWars::ThoughtProcess_Manual(&optimusPi);
  //PiWars::ThoughtProcess *process = new PiWars::ThoughtProcess_Proximity(&optimusPi);
  PiWars::ThoughtProcess *process = new PiWars::ThoughtProcess_LineFollower(&optimusPi);
  
  while(!process->available()) {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }

  if(process->prepare()) {
    process->run();
  }
  
  return EXIT_SUCCESS;
}

