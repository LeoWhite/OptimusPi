/**
 * This ThoughtProcess is responsible for guiding the robot through
 * the three point turn challenge
 *
 * http://piwars.org/2015-competition/challenges/three-point-turn/
 *
 * This is the 'simple' implementation that uses dead-reckoning to
 * complete the course
 */


#ifndef _PIWARS_THOUGHT_PROCESS_THREE_POINT_TURN_SIMPLE_H
#define _PIWARS_THOUGHT_PROCESS_THREE_POINT_TURN_SIMPLE_H

#include "ThoughtProcess.h"

namespace PiWars {

class SensorRTIMU;

class ThoughtProcess_ThreePointTurnSimple : public ThoughtProcess {
  public:
    ThoughtProcess_ThreePointTurnSimple(PiWars *robot);
    ~ThoughtProcess_ThreePointTurnSimple();

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
    // @param backwards If true drive backwards instead of forwards
    // @param seconds The number of seconds to drive for
    void driveForDuration(std::atomic<bool> &running, const bool backwards, const float seconds);

    // Turns the robot left 90 degrees
    //
    // @param running If this becomes false then exit early
    void turnLeft(std::atomic<bool> &running);
};

}

#endif
