#include <cstdlib>
#include <iostream>
#include <poll.h>

#include "OptimusPiConfig.h"
#include "PiWars.h"
#include "InputDevice.h"
#include "InputEvent.h"
#include "Powertrain.h"
#include "SensorVL6180.h"

int main(int argc,char *argv[]) {
  std::cout << "Hello World!" << std::endl;
  std::cout << "Version " << OptimusPi_VERSION_MAJOR << "." << OptimusPi_VERSION_MINOR << std::endl;  
  std::string joystickPath("/dev/input/by-id/usb-Sony_PLAYSTATION_R_3_Controller-event-joystick");
  PiWars::InputDevice joystick(joystickPath);
  PiWars::PiWars optimusPi;
  PiWars::SensorVL6180 vl6180;

  PiWars::InputEventQueue inputQueue;
        
  // Attach the input event queue
  joystick.setEventQueue(inputQueue);

  std::cout << "Device name " << joystick.getName() << " axis : "<< joystick.getNumAxes() << " buttons: " << joystick.getNumButtons() << std::endl;
  if(vl6180.enable() && joystick.claim()) {
    struct pollfd fds[2];
    float leftMotor = 0.0, rightMotor = 0.0;
  
    // Query the file descriptor so we can correctly wait for events
    fds[0].fd = inputQueue.getFD();
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    
    std::cout << "Joystick claimed" << std::endl;

    // Sleep for messages
    while(int result = poll(fds, 1, -1)) {
      // Something went wrong (FD got closed, device was removed)
      // so we simply exit out
      if(-1 == result) {
        std::cerr << "Poll returned error" << std::endl;
        return EXIT_FAILURE;
      }
      else {
        bool updateMotor = false;

        // Anyting else should be from the input device
        if(fds[0].revents & POLLIN) {
          int rc;
          PiWars::InputEvent event(0,0);
  
          // Tell poll that we have processed the event
          fds[0].revents = 0;
  
          // Clear out any queued up input events
          while(!inputQueue.empty()) {
            inputQueue.pop(event);
                          
            if(PiWars::InputEventType::AXIS == event.getType()) {
              
              if(ABS_Y == event.getCode()) {
                // We invert the Y axis
                leftMotor = -(event.getAxisValue());
                updateMotor = true;
              }
              else if(ABS_RZ == event.getCode()) {
                // We invert the Y axis
                rightMotor = -(event.getAxisValue());
                updateMotor = true;
              }
            }
          }
        }

        if(updateMotor) {
          std::cout << "Setting motor " << leftMotor << " - " <<  rightMotor << std::endl;;
          optimusPi.powertrain()->setPower(leftMotor, rightMotor);
          std::cout << "Range: " << (int)vl6180.range() << "mm" << std::endl;;
    
       }
        
      }      
    }

    optimusPi.powertrain()->stop();
    
    std::cout << "Releasing joystick" << std::endl;
    joystick.release();
  }
  else {
    std::cout << "Failed to claim joystick" << std::endl;
  }


  return EXIT_SUCCESS;
}

