/**
 * The ThoughtProcess class represents a specific control mode for the
 * robot. Be it the code to allow the robot to be manually driven around,
 * or the code to complete a challenge.
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
    
    // For manual control we need access to an appropiate
    // InputDevice (e.g. a joystick).
    bool available();
    
    // For manual control we need to claim the joystick
    // and MotorDriver
    bool prepare();
    
    // Whilst running we need take input from the joystick
    // and feed it to the control process
    void run();
    
    // Force the ThoughtProcess to stop
    void stop();

  private:
    InputDevice *_joystick;
    InputEventQueue *_inputQueue;
};

}

#endif