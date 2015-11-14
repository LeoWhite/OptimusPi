/**
 * This ThoughtProcess is responsible for controlling the robot
 * as it attempts the complete the Proximity alert challenge
 * http://piwars.org/2015-competition/challenges/proximity-alert/
 *
 * Currently we make use of the VL6180 sensor for range detection.
 */

#include <cstdint>
#include <cstddef>
#include <unistd.h>

#include "ThoughtProcess.h"
#include "ThoughtProcess_Proximity.h"
#include "SensorVL6180.h"
#include "PiWars.h"
#include "Powertrain.h"
#include <iostream>

namespace PiWars {


ThoughtProcess_Proximity::ThoughtProcess_Proximity(PiWars *robot) : ThoughtProcess(robot), _vl6180(new SensorVL6180()) {
}

ThoughtProcess_Proximity::~ThoughtProcess_Proximity() {
  if(_vl6180) {
    _vl6180->disable();
    delete _vl6180;
    _vl6180 = nullptr;
  }
}

const std::string &ThoughtProcess_Proximity::name() {
  static std::string name("Proximity");

  return name;
}

bool ThoughtProcess_Proximity::available() {
  return _vl6180->exists();
}

bool ThoughtProcess_Proximity::prepare() {
  return _vl6180->enable();
}

void ThoughtProcess_Proximity::run(std::atomic<bool> &running) {
  float lastPower = -1.0;

  while(running.load()) {
    // Read in the current range
    uint8_t range = _vl6180->range();
    float power = 0.0;

    // Less than 20mm, so stop now (It takes some distance to stop)
    if(range <= 20) {
      // Stop!
      power = 0.0;

      // and we've completed
      running = false;
    }
    // Within 50mm
    else if(range < 50) {
      // getting closer, start to slow down
      power = 0.18;
    }
    // 50-100mm
    else if(range < 100) {
      // Drop to quarter speed
      power = 0.30;
    }
    // Long way to go yet!
    else {
      // Proceed forwards at half speed
      power = 0.50;
    }

    // Set the motors
    if(lastPower != power) {
      robot()->powertrain()->setPower(power, power);
      lastPower = power;
    }

    // Let the robot actually move
    std::this_thread::sleep_for (std::chrono::microseconds(10));
  }

  // Stop the robot, hopefully close to the wall!
  robot()->powertrain()->stop();
  
  // Disable the sensor
  _vl6180->disable();
}

}
