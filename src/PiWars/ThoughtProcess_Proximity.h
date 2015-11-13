/**
 * This ThoughtProcess is responsible for controlling the robot
 * as it attempts the complete the Proximity alert challenge
 * http://piwars.org/2015-competition/challenges/proximity-alert/
 *
 * Currently we make use of the VL6180 sensor for range detection.
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

    // Implementation of the virtual APIs
    const std::string &name();
    bool available();
    bool prepare();
    void run(std::atomic<bool> &running);

  private:
    SensorVL6180 *_vl6180; //<! We use a VL6180 for range detection
};

}

#endif
