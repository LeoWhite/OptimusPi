#include <cstdlib>
#include <iostream>
#include "OptimusPiConfig.h"
#include "InputDevice.h"
#include "InputEvent.h"
#include "Powertrain.h"

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
    std::this_thread::sleep_for (std::chrono::seconds(1));

    std::cout << "Releasing joystick" << std::endl;
    joystick.release();
  }
  else {
    std::cout << "Failed to claim joystick" << std::endl;
  }

  // Motor testing...
  std::cout << "creating powertrain" << std::endl;

  PiWars::Powertrain powertrain;

  float left = 1, right = 1;
  
  powertrain.getPower(left, right);
  powertrain.stop();
  powertrain.setPower(0.25, 0.75);
    std::this_thread::sleep_for (std::chrono::seconds(1));
  powertrain.setPower(-0.25, 0.75);
    std::this_thread::sleep_for (std::chrono::seconds(1));
  powertrain.setPower(-0.50, -0.66);
    std::this_thread::sleep_for (std::chrono::seconds(1));
  powertrain.stop();

  powertrain.setLimiter(0.75);
  powertrain.setPower(1.0, 0.50);
    std::this_thread::sleep_for (std::chrono::seconds(1));

  powertrain.setLimiter(0.50);
  powertrain.setPower(1.0, 0.50);
    std::this_thread::sleep_for (std::chrono::seconds(1));
  

    std::cout << "Left = " << left << "Right " << right << std::endl;

  return EXIT_SUCCESS;
}

