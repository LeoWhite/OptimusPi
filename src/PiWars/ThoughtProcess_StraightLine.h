/**
 * The ThoughtProcess class represents a specific control mode for the
 * robot. Be it the code to allow the robot to be manually driven around,
 * or the code to complete a challenge.
 */

#ifndef _PIWARS_THOUGHT_PROCESS_STRAIGHT_LINE_H
#define _PIWARS_THOUGHT_PROCESS_STRAIGHT_LINE_H

#include "ThoughtProcess.h"

namespace PiWars {

class SensorRTIMU;

class ThoughtProcess_StraightLine : public ThoughtProcess {
  public:
    ThoughtProcess_StraightLine(PiWars *robot);
    ~ThoughtProcess_StraightLine();
    
    // Proximity checks can only occur when the appropiate
    // sensors are available.
    bool available();
    
    // Enable the Sensor and Powertrain
    bool prepare();
    
    void run();
    
    // Force the ThoughtProcess to stop
    void stop();

  private:
    SensorRTIMU *_rtimu; //<! We use the RTIMU for detecting direction
};

}

#endif
