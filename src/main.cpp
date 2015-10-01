#include <cstdlib>
#include <iostream>
#include "OptimusPiConfig.h"
#include "InputDevice.h"
#include "InputEvent.h"


int main(int argc,char *argv[]) {
  std::cout << "Hello World!" << std::endl;
  std::cout << "Version " << OptimusPi_VERSION_MAJOR << "." << OptimusPi_VERSION_MINOR << std::endl;  
  std::string joystickPath("/dev/input/by-id/usb-Sony_PLAYSTATION_R_3_Controller-event-joystick");
  PiWars::InputDevice joystick(joystickPath);

  std::shared_ptr<PiWars::InputEventQueue> inputQueue;
    
  std::cout << "Device name " << joystick.getName() << " axis : "<< joystick.getNumAxes() << " buttons: " << joystick.getNumButtons() << std::endl;
  if(joystick.claim()) {
    std::cout << "Joystick claimed" << std::endl;

    // Sleep so the input thread doesn't immediately get told to exit
    std::this_thread::sleep_for (std::chrono::seconds(5));

    std::cout << "Releasing joystick" << std::endl;
    joystick.release();
  }
  else {
    std::cout << "Failed to claim joystick" << std::endl;
  }

  return EXIT_SUCCESS;
}

