/**
 * The ThoughtProcess class represents a specific control mode for the
 * robot. Be it the code to allow the robot to be manually driven around,
 * or the code to complete a challenge.
 */

#ifndef _PIWARS_THOUGHT_PROCESS_H
#define _PIWARS_THOUGHT_PROCESS_H

#include <cstdint>
#include <cstddef>
#include <unistd.h>
#include <sys/eventfd.h>

namespace PiWars {
  class PiWars;
  
class ThoughtProcess {
  public:
    //
    // @param robot The PiWars robot this ThoughtProcess 
    //              is part of
    ThoughtProcess(PiWars *robot) : _robot(robot) {
    }
    
    // Checks if this 'ThoughtProcess' is avaible for use.
    // Some processes may have requirements for specific
    // sensors, so if they are not attached then the process
    // can't be run
    virtual bool available() = 0;
    
    // Prepare's the thought process ready for use
    virtual bool prepare() = 0;
    
    // Run the main 'ThoughtProcess' loop
    virtual void run() = 0;
    
    // Force the ThoughtProcess to stop
    virtual void stop() = 0;

  protected:
    PiWars *robot() { return _robot; }

  private:
    PiWars *_robot;
};

}

#endif