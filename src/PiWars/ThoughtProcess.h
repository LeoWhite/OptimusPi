/**
 * The ThoughtProcess class represents a specific control mode for the
 * robot. Be it the code to allow the robot to be manually driven around,
 * or the code to complete a challenge.
 */

#ifndef _PIWARS_THOUGHT_PROCESS_H
#define _PIWARS_THOUGHT_PROCESS_H

#include <atomic>
#include <cstdint>
#include <cstddef>
#include <unistd.h>
#include <sys/eventfd.h>
#include <memory>
#include <string>
#include <vector>

namespace PiWars {

// Forward declared classes
class PiWars;

class ThoughtProcess {
  public:
    typedef std::shared_ptr<ThoughtProcess> ptr;
    typedef std::vector<ptr> vector;
    typedef vector::iterator iterator;

    // Initialse the ThoughtProcess, caching the robot it
    // will be running in.
    //
    // @param robot The PiWars robot this ThoughtProcess
    //              is part of
    ThoughtProcess(PiWars *robot) : _robot(robot) {
    }

    // Returns the name of the ThoughtProcess
    virtual const std::string &name() = 0;

    // Checks if this 'ThoughtProcess' is avaible for use.
    // Some processes may have requirements for specific
    // sensors, so if they are not attached then the process
    // can't be run
    virtual bool available() = 0;

    // Prepares the thought process ready for use
    virtual bool prepare() = 0;

    // Run the main 'ThoughtProcess' loop
    virtual void run(std::atomic<bool> &running) = 0;

  protected:
    // return the cached Robot object
    PiWars *robot() { return _robot; }

  private:
    PiWars *_robot; //<! The PiWars robot the ThoughtProcesses run on
};

}

#endif
