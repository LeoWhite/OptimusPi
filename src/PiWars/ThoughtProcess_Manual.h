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
    
    const std::string &name();

    bool available();
    
    bool prepare();
    
    void run(std::atomic<bool> &running);
    
    void stop();

  private:
    InputDevice *_joystick;
    InputEventQueue *_inputQueue;
};

}

#endif
