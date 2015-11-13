/**
 * This ThoughtProcess is responsible for allowing the user
 * to manally control the robot via a PS3 controller.
 */

#include <cstdint>
#include <cstddef>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/stat.h>

#include "ThoughtProcess.h"
#include "ThoughtProcess_Manual.h"
#include "InputDevice.h"
#include "InputEvent.h"
#include "PiWars.h"
#include "Powertrain.h"
#include <iostream>

namespace PiWars {

// The (currently) hardcoded path of the joystick
// IMPROVE: We should use the InputManager to look this up
static std::string joyStickPath = "/dev/input/event1";

ThoughtProcess_Manual::ThoughtProcess_Manual(PiWars *robot) : ThoughtProcess(robot), _joystick(nullptr), _inputQueue(nullptr) {
}

ThoughtProcess_Manual::~ThoughtProcess_Manual() {
  if(_joystick) {
    _joystick->release();
    delete _joystick;
    _joystick = nullptr;
  }

  if(_inputQueue) {
    delete _inputQueue;
    _inputQueue = nullptr;
  }
}

const std::string &ThoughtProcess_Manual::name() {
  static std::string name("Manual");

  return name;
}

bool ThoughtProcess_Manual::available() {
  bool available = false;
  struct stat buffer;

  // IMPROVE: We should be dynamically finding a joystick, instead of hardcoding a path
  if(stat(joyStickPath.c_str(), &buffer) == 0) {
    available = true;
  }

  return available;
}

bool ThoughtProcess_Manual::prepare() {
  bool prepared = false;

  // Create the InputDevice, connect up the event queue and claim
  // the joystick
  _joystick = new InputDevice(joyStickPath);
  _inputQueue = new InputEventQueue();

  _joystick->setEventQueue(*_inputQueue);

  if(_joystick->claim()) {
    prepared = true;
  }
  else {
    delete _joystick;
    _joystick = nullptr;
  }

  return prepared;
}

void ThoughtProcess_Manual::run(std::atomic<bool> &running) {
  struct pollfd fds[2];
  float leftMotor = 0.0, rightMotor = 0.0;

  // Query the file descriptor so we can correctly wait for events
  fds[0].fd = _inputQueue->getFD();
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  while(int result = poll(fds, 1, -1) && running.load()) {
    // Something went wrong (FD got closed, device was removed)
    // so we simply exit out
    if(-1 == result) {
      std::cerr << __func__ << ": Poll returned error" << std::endl;
      break;
    }
    else {
      bool updateMotor = false;

      // Anyting else should be from the input device
      if(fds[0].revents & POLLIN) {
        int rc;
        InputEvent event(0,0);

        // Tell poll that we have processed the event
        fds[0].revents = 0;

        // Clear out any queued up input events
        while(!_inputQueue->empty()) {
          _inputQueue->pop(event);

          if(InputEventType::AXIS == event.getType()) {

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
        robot()->powertrain()->setPower(leftMotor, rightMotor);
      }
    }
  }

  // Ensure the robot is stopped, and release the joystick
  robot()->powertrain()->stop();
  _joystick->release();
}

}
