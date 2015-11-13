/**
 * The entry point of the programme. Here we initialise the PiWars library
 * and select the various ThoughtProcesses that we want the robot to be
 * able to run.
 */
 
#include <cstdlib>
#include <iostream>

#include "OptimusPiConfig.h"
#include "PiWars.h"
#include "Brains.h"
#include "ThoughtProcess_Manual.h"
#include "ThoughtProcess_Proximity.h"
#include "ThoughtProcess_LineFollower.h"
#include "ThoughtProcess_StraightLine.h"
#include "ThoughtProcess_ThreePointTurn.h"

int main(int argc,char *argv[]) {
  PiWars::PiWars optimusPi;

  std::cout << "Optimus Pi Version " << OptimusPi_VERSION_MAJOR << "." << OptimusPi_VERSION_MINOR << std::endl;  

  // Load the PiWars up with all the ThoughtProcesses we need
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_Manual>(&optimusPi));
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_LineFollower>(&optimusPi));
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_Proximity>(&optimusPi));
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_StraightLine>(&optimusPi));
  optimusPi.brains()->addThoughtProcess(std::make_shared<PiWars::ThoughtProcess_ThreePointTurn>(&optimusPi));

  // Let the robot run!  
  optimusPi.run();

  return EXIT_SUCCESS;
}

