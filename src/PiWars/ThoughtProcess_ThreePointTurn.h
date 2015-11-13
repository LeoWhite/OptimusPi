/**
 * This ThoughtProcess is responsible for guiding the robot through
 * the three point turn challenge
 *
 * http://piwars.org/2015-competition/challenges/three-point-turn/
 *
 * Currently we make use of the SenseHAT, via the RTIMU library, to
 * determine our current heading, and dead reckoning to
 * drive round the course.
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

    // Implementation of the virtual APIs
    const std::string &name();
    bool available();
    bool prepare();
    void run(std::atomic<bool> &running);

  private:
    // Drive on the specific heading, in the specified direction for the
    // specified amount of time.
    //
    // @param running If this becomes false then exit early
    // @param heading The heading to maintain from 0 to 360
    // @param backwards If true drive backwards instead of forwards
    // @param seconds The number of seconds to drive for
    void driveForDuration(std::atomic<bool> &running, const float heading, const bool backwards, const float seconds);

    // Turns the robot left until it reaches the specified heading
    //
    // @param running If this becomes false then exit early
    // @param The target heading from 0-360
    void turnLeft(std::atomic<bool> &running, const float heading);

    SensorRTIMU *_rtimu; //<! We use the RTIMU for detecting direction
};

}

#endif
