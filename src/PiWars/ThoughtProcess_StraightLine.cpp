/**
 * This ThoughtProcess is responsible for keeping the robot
 * on a straight line as it attempts the straight line
 * speed challenge
 * http://piwars.org/2015-competition/challenges/straight-line-speed-test/
 *
 * Currently we make use of the SenseHAT, via the RTIMU library, to
 * determine our current heading.
 */

#include <cstdint>
#include <cstddef>
#include <unistd.h>

#include "ThoughtProcess.h"
#include "ThoughtProcess_StraightLine.h"
#include "SensorRTIMU.h"
#include "PiWars.h"
#include "Powertrain.h"
#include <iostream>

namespace PiWars {


ThoughtProcess_StraightLine::ThoughtProcess_StraightLine(PiWars *robot) : ThoughtProcess(robot), _rtimu(new SensorRTIMU()) {
}

ThoughtProcess_StraightLine::~ThoughtProcess_StraightLine() {
  if(_rtimu) {
    _rtimu->disable();
    delete _rtimu;
    _rtimu = nullptr;
  }
}

const std::string &ThoughtProcess_StraightLine::name() {
  static std::string name("StraightLine");

  return name;
}

bool ThoughtProcess_StraightLine::available() {
  return _rtimu->exists();
}

bool ThoughtProcess_StraightLine::prepare() {
  return _rtimu->enable();
}

void ThoughtProcess_StraightLine::run(std::atomic<bool> &running) {
  float lastPowerLeft = -1.0, lastPowerRight = -1.0;
  float pitch, roll, yaw;

  std::chrono::time_point<std::chrono::system_clock> start, end;
  float heading;

  // Let the sensor settle down
  std::this_thread::sleep_for (std::chrono::seconds(2));

  // Get the heading that we want to maintain
  _rtimu->fusion(pitch, roll, yaw);
  heading = yaw + 180;

  std::cout << __func__ << "Selected heading " << heading << std::endl;

  // Start the motors off at 50% so we don't pull too much current when we jump to 66%
  // IMPROVE: Update the PowerTrain, or Arduino code, to handle this ramp up
  // automatically
  robot()->powertrain()->setPower(0.50, 0.50);

  // Note the start time
  start = std::chrono::system_clock::now();

  while(running.load())
  {
    float currentHeading, offset, powerLeft, powerRight;

    // Get the heading that we want to maintain
    _rtimu->fusion(pitch, roll, yaw);

    // Work out our offset versus the heading
    currentHeading = yaw + 180;

    // What's the difference?
    offset = currentHeading - heading;

    // Output the heading, followed by a line feed so the next heading will overwrite it
    std::cout << "Current heading " << currentHeading << " offset " << offset << "\r" << std::flush;

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
      robot()->powertrain()->setPower(powerLeft, powerRight);
      //std::cerr << "setting power to " << powerLeft << ":" << powerRight << std::endl;
      lastPowerLeft = powerLeft;
      lastPowerRight = powerRight;
    }

    // Let the robot actually move
    std::this_thread::sleep_for (std::chrono::microseconds(100));

    end = std::chrono::system_clock::now();
    std::chrono::duration<float> duration = end - start;

    // Travel forwards for 5 seconds
    // IMPROVE: Need to measure distance travelled, or use the range sensor to detect the end
    if(duration.count() >= 5.0){
      break;
    }
  }

  // and stop the robot
  robot()->powertrain()->stop();

  // Release the sensor
  _rtimu->disable();
}

}
