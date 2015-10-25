/**
 * The ThoughtProcess class represents a specific control mode for the
 * robot. Be it the code to allow the robot to be manually driven around,
 * or the code to complete a challenge.
 */

#ifndef _PIWARS_THOUGHT_PROCESS_LINE_FOLLOWER_H
#define _PIWARS_THOUGHT_PROCESS_LINE_FOLLOWER_H

#include "ThoughtProcess.h"

namespace PiWars {

class SensorQTR8RC;

class ThoughtProcess_LineFollower : public ThoughtProcess {
  public:
    ThoughtProcess_LineFollower(PiWars *robot);
    ~ThoughtProcess_LineFollower();
    
    // Proximity checks can only occur when the appropiate
    // sensors are available.
    bool available();
    
    // Enable the Sensor and Powertrain
    bool prepare();
    
    void run();
    
    // Force the ThoughtProcess to stop
    void stop();

  private:
    SensorQTR8RC *_qtr8rc; //<! We use the QTR8RC for sensing the line
};

}

#endif
