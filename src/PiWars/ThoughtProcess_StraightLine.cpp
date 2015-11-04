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

bool ThoughtProcess_StraightLine::available() {
  return _rtimu->exists();
}

bool ThoughtProcess_StraightLine::prepare() {
  return _rtimu->enable();
}

void ThoughtProcess_StraightLine::run() {
  bool running = true;
  float lastPowerLeft = -1.0, lastPowerRight = -1.0;
  float pitch, roll, yaw;
  
  float heading;
  
  // Let the sensor settle down
  std::this_thread::sleep_for (std::chrono::seconds(2));

  // Get the heading that we want to maintain
  _rtimu->fusion(pitch, roll, yaw);
  heading = yaw + 180;
  
  while(true) {
    float currentHeading, offset, powerLeft, powerRight;
    
    // Get the heading that we want to maintain
    _rtimu->fusion(pitch, roll, yaw);

    // Work out our offset versus the heading
    currentHeading = yaw + 180;
    
    // What's the difference?
    offset = currentHeading - heading;

    // Very simple checks    
    if(offset < 1.0) {
      // We want to move left slightly
      powerLeft = 0.35;
      powerRight = 0.50;
    }
    else if(offset > 1.0) {
      // Move right
      powerLeft = 0.50;
      powerRight = 0.35;
    }
    else {
      // straight on!
      powerLeft = 0.50;
      powerRight = 0.50;
    }
    
    // Set the motors
    if(lastPowerLeft != powerLeft || lastPowerRight != powerRight) {
      robot()->powertrain()->setPower(powerLeft, powerRight);
      std::cerr << "setting power to " << powerLeft << ":" << powerRight << std::endl;
      lastPowerLeft = powerLeft;
      lastPowerRight = powerRight;
    } 

    // Let the robot actually move
    std::this_thread::sleep_for (std::chrono::microseconds(200));
  }
 
  robot()->powertrain()->stop();
}

void ThoughtProcess_StraightLine::stop() {
}

}
