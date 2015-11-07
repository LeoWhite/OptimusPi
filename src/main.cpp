#include <cstdlib>
#include <iostream>
#include <poll.h>

#include "OptimusPiConfig.h"
#include "PiWars.h"
#include "Brains.h"
#include "ThoughtProcess_Manual.h"
#include "ThoughtProcess_Proximity.h"
#include "ThoughtProcess_LineFollower.h"
#include "ThoughtProcess_StraightLine.h"
#include "ThoughtProcess_ThreePointTurn.h"

int main(int argc,char *argv[]) {
  std::cout << "Hello World!" << std::endl;
  std::cout << "Version " << OptimusPi_VERSION_MAJOR << "." << OptimusPi_VERSION_MINOR << std::endl;  
  PiWars::PiWars optimusPi;

  // Load the PiWars up with all the ThoughtProcesses we need
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_Manual>(&optimusPi));
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_LineFollower>(&optimusPi));
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_Proximity>(&optimusPi));
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_StraightLine>(&optimusPi));
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_ThreePointTurn>(&optimusPi));
  
  // List out all the available processes
  for(auto n : optimusPi.brains()->processes()) {
    if(n->available()) {
      std::cerr << "Process " << n->name() << " is available" << std::endl;
    }
  }
  
  optimusPi.brains()->forEachThoughtProcess([](PiWars::ThoughtProcess::ptr n)
    {
      if(n->available()) { 
        std::cerr << "Process " << n->name() << " is available" << std::endl;
      }
    });
    
  optimusPi.brains()->enableThoughtProcess(3);
  
/*  std::cout << process->name() << std::endl;
  
  while(!process->available()) {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }

  if(process->prepare()) {
    process->run();
  }
  */
  
  return EXIT_SUCCESS;
}

