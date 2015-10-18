/**
 * The ThoughtProcess class represents a specific control mode for the
 * robot. Be it the code to allow the robot to be manually driven around,
 * or the code to complete a challenge.
 */

#ifndef _PIWARS_THOUGHT_PROCESS_PROXIMITY_H
#define _PIWARS_THOUGHT_PROCESS_PROXIMITY_H

#include "ThoughtProcess.h"

namespace PiWars {

class SensorVL6180;

class ThoughtProcess_Proximity : public ThoughtProcess {
  public:
    ThoughtProcess_Proximity(PiWars *robot);
    ~ThoughtProcess_Proximity();
    
    // Proximity checks can only occur when the appropiate
    // sensors are available.
    bool available();
    
    // Enable the Sensor and Powertrain
    bool prepare();
    
    void run();
    
    // Force the ThoughtProcess to stop
    void stop();

  private:
    SensorVL6180 *_vl6180; //<! We use a VL6180 for range detection
};

}

#endif
