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

#include <cstdint>
#include <cstddef>
#include <unistd.h>

#include "ThoughtProcess.h"
#include "ThoughtProcess_ThreePointTurn.h"
#include "SensorRTIMU.h"
#include "PiWars.h"
#include "Powertrain.h"
#include <iostream>

namespace PiWars {


ThoughtProcess_ThreePointTurn::ThoughtProcess_ThreePointTurn(PiWars *robot) : ThoughtProcess(robot), _rtimu(new SensorRTIMU()) {
}

ThoughtProcess_ThreePointTurn::~ThoughtProcess_ThreePointTurn() {
  if(_rtimu) {
    _rtimu->disable();
    delete _rtimu;
    _rtimu = nullptr;
  }
}

const std::string &ThoughtProcess_ThreePointTurn::name() {
  static std::string name("ThreePointTurn");

  return name;
}

bool ThoughtProcess_ThreePointTurn::available() {
  return _rtimu->exists();
}

bool ThoughtProcess_ThreePointTurn::prepare() {
  return _rtimu->enable();
}

void ThoughtProcess_ThreePointTurn::run(std::atomic<bool> &running) {
  float pitch, roll, yaw;

  std::chrono::time_point<std::chrono::system_clock> start, end;
  float heading;

  // Let the sensor settle down
  std::this_thread::sleep_for (std::chrono::seconds(2));

  // Get the heading that we want to maintain
  _rtimu->fusion(pitch, roll, yaw);
  heading = yaw + 180;

  std::cerr << "Selected heading " << heading << std::endl;

  // Start the motors off at 50% so we don't pull too much current when we jump to 66%
  robot()->powertrain()->setPower(0.50, 0.50);

  driveForDuration(running, heading, false, 7.5);

  // Turn left
  heading = heading - 90;

  if(heading < 0) {
    heading = 360 + heading;
  }

  std::cerr << "Turning left to " << heading << std::endl;
  turnLeft(running, heading);

  // Drive forwards again

  std::cerr << "forwards" << std::endl;
  driveForDuration(running, heading, false, 1.4f);

  // Drive backwards
  driveForDuration(running, heading, true, 3.0f);

  // Forwards again
  driveForDuration(running, heading, false, 1.5f);

  // Turn left again
  heading = heading - 90;

  if(heading < 0) {
    heading = 360 + heading;
  }

  turnLeft(running, heading);

  // and finally head home
  driveForDuration(running, heading, false, 7.5f);

  // Release the sensor
  _rtimu->disable();

  std::cerr << std::endl << "Done!" << std::endl;
}

void ThoughtProcess_ThreePointTurn::driveForDuration(std::atomic<bool> &running, const float heading, const bool backwards, const float seconds)
{
  std::chrono::time_point<std::chrono::system_clock> start, end;
  float lastPowerLeft = 0.0f, lastPowerRight = 0.0f;

  // Note the start time
  start = std::chrono::system_clock::now();

  // Move forwards on the current heading
  while(running.load()) {
    float currentHeading, offset, powerLeft, powerRight;
    float pitch, roll, yaw;

    // Get the heading that we want to maintain
    _rtimu->fusion(pitch, roll, yaw);

    // Work out our offset versus the heading
    currentHeading = yaw + 180;

    // What's the difference?
    offset = currentHeading - heading;

    std::cerr << "Current heading " << currentHeading << " offset " << offset << "\r" << std::flush;
    // Very simple checks
    if(offset > 0) {
      // We want to move left slightly
      powerLeft = 0.55;
      powerRight = 0.66;
    }
    else if(offset < 0) {
      // Move right
      powerLeft = 0.66;
      powerRight = 0.55;
    }
    else {
      // straight on!
      powerLeft = 0.66;
      powerRight = 0.66;
    }

    // Set the motors
    if(lastPowerLeft != powerLeft || lastPowerRight != powerRight) {
      // Are we moving backwards?
      if(backwards) {
        float temp = powerLeft;

        powerLeft = -powerRight;
        powerRight = -temp;
      }

      robot()->powertrain()->setPower(powerLeft, powerRight);
      //std::cerr << "setting power to " << powerLeft << ":" << powerRight << std::endl;
      lastPowerLeft = powerLeft;
      lastPowerRight = powerRight;
    }

    // Let the robot actually move
    std::this_thread::sleep_for (std::chrono::microseconds(100));

    end = std::chrono::system_clock::now();
    std::chrono::duration<float> duration = end - start;


    if(duration.count() >= seconds){
      break;
    }
  }

  // and stop
  robot()->powertrain()->stop();
}

void ThoughtProcess_ThreePointTurn::turnLeft(std::atomic<bool> &running, const float heading) {
  // Start turning left
  robot()->powertrain()->setPower(-0.50, 0.50);
  while(running.load())
  {
    float currentHeading, offset;
    float pitch, roll, yaw;

    // Get the heading that we want to maintain
    _rtimu->fusion(pitch, roll, yaw);

    // Work out our offset versus the heading
    currentHeading = yaw + 180;

    // What's the difference?
    offset = currentHeading - heading;

    std::cerr << "Current heading " << currentHeading << " offset " << offset << "\r" << std::flush;
    // Very simple checks
    if(offset > 0.25) {
      // Keep turning left
  robot()->powertrain()->setPower(-0.50, 0.50);
    }
    else {
      // We've arrived
      break;
    }

    // Let the robot actually move
    std::this_thread::sleep_for (std::chrono::microseconds(100));
  }

  // Stop
  robot()->powertrain()->stop();
}

}

