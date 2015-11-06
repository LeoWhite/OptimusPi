/**
 * This ThoughtProcess is responsible for keeping the robot
 * on a straight line as it attempts the straight line 
 * speed challenge 
 * http://piwars.org/2015-competition/challenges/straight-line-speed-test/
 *
 * Currently we make use of the SenseHAT, via the RTIMU library, to
 * determine our current heading.
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
    
    bool available();
    
    bool prepare();
    
    void run();
    
    void stop();

  private:
    SensorRTIMU *_rtimu; //<! We use the RTIMU for detecting direction
};

}

#endif
