/**
 * The ThoughtProcess class represents a specific control mode for the
 * robot. Be it the code to allow the robot to be manually driven around,
 * or the code to complete a challenge.
 */

#ifndef _PIWARS_THOUGHT_PROCESS_THREE_POINT_TURN_H
#define _PIWARS_THOUGHT_PROCESS_THREE_POINT_TURN_H

#include "ThoughtProcess.h"

namespace PiWars {

class SensorRTIMU;

class ThoughtProcess_ThreePointTurn : public ThoughtProcess {
  public:
    ThoughtProcess_ThreePointTurn(PiWars *robot);
    ~ThoughtProcess_ThreePointTurn();
    
    // Proximity checks can only occur when the appropiate
    // sensors are available.
    bool available();
    
    // Enable the Sensor and Powertrain
    bool prepare();
    
    void run();
    
    // Force the ThoughtProcess to stop
    void stop();

  private:
    // Drive on the specific heading, in the specified direction for the 
    // specified amount of time.
    //
    // @param heading The heading to maintain from 0 to 360
    // @param backwards If true drive backwards instead of forwards
    // @param seconds The number of seconds to drive for
    void driveForDuration(const float heading, const bool backwards, const float seconds);
  
    // Turns the robot left until it reaches the specified heading
    //
    // @param The target heading from 0-360    
    void turnLeft(const float heading);

    SensorRTIMU *_rtimu; //<! We use the RTIMU for detecting direction
};

}

#endif
