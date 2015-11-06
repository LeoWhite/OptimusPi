/**
 * This ThoughtProcess is responsible for guiding the robot through
 * the Line following challenge as defined at
 * http://piwars.org/2015-competition/challenges/line-follower/
 */

#ifndef _PIWARS_THOUGHT_PROCESS_LINE_FOLLOWER_H
#define _PIWARS_THOUGHT_PROCESS_LINE_FOLLOWER_H

#include "ThoughtProcess.h"

namespace PiWars {

// Forward declare the classes we'll be using
class SensorQTR8RC;

class ThoughtProcess_LineFollower : public ThoughtProcess {
  public:
    ThoughtProcess_LineFollower(PiWars *robot);
    ~ThoughtProcess_LineFollower();
    
    const std::string &name();

    bool available();
    
    bool prepare();
    
    void run();
    
    void stop();

  private:
    SensorQTR8RC *_qtr8rc; //<! We use the QTR8RC for sensing the line
};

}

#endif
