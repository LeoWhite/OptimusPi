/**
 * This ThoughtProcess is responsible for allowing the user
 * to manally control the robot via a PS3 controller.
 */

#ifndef _PIWARS_THOUGHT_PROCESS_MANUAL_H
#define _PIWARS_THOUGHT_PROCESS_MANUAL_H

#include "ThoughtProcess.h"

namespace PiWars {
  class InputDevice;
  class InputEventQueue;

class ThoughtProcess_Manual : public ThoughtProcess {
  public:
    ThoughtProcess_Manual(PiWars *robot);
    ~ThoughtProcess_Manual();

    // Implementation of the virtual APIs
    const std::string &name();
    bool available();
    bool prepare();
    void run(std::atomic<bool> &running);

  private:
    InputDevice *_joystick; //<! The InputDevice that is controlling the robot
    InputEventQueue *_inputQueue; //<! InputQueue used to buffer incoming input
};

}

#endif
