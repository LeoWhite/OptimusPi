/**
 * This ThoughtProcess is responsible for guiding the robot through
 * the Line following challenge as defined at
 * http://piwars.org/2015-competition/challenges/line-follower/
 */

#include <cstdint>
#include <cstddef>
#include <unistd.h>

#include "ThoughtProcess.h"
#include "ThoughtProcess_LineFollower.h"
#include "SensorQTR8RC.h"
#include "PiWars.h"
#include "Powertrain.h"
#include <iostream>

namespace PiWars {

ThoughtProcess_LineFollower::ThoughtProcess_LineFollower(PiWars *robot) : ThoughtProcess(robot), _qtr8rc(new SensorQTR8RC()) {
}

ThoughtProcess_LineFollower::~ThoughtProcess_LineFollower() {
  if(_qtr8rc) {
    _qtr8rc->disable();
    delete _qtr8rc;
    _qtr8rc = nullptr;
  }
}

const std::string &ThoughtProcess_LineFollower::name() {
  static std::string name("LineFollower");

  return name;
}

bool ThoughtProcess_LineFollower::available() {
  // The QTR8RC Sensor must be connected for this ThoughtProcess
  // to run
  return _qtr8rc->exists();
}

bool ThoughtProcess_LineFollower::prepare() {
  return _qtr8rc->enable();
}

void ThoughtProcess_LineFollower::run(std::atomic<bool> &running) {
  float lastPowerLeft = -1.0, lastPowerRight = -1.0;
  uint16_t position = 0;

  while(running.load()) {
    uint16_t sensorDiff[8] = {0};
    float powerLeft, powerRight;
    uint16_t temp;
    
    // Read in the sensor details
    if(_qtr8rc->readLine(sensorDiff, temp)) {
      uint16_t newPosition = 0;
      
      // Very simple implemenation

      for(size_t i = 0; i < 8; i++) {
        if(sensorDiff[i] >= 500) {
          newPosition |= (1 << i);
        }
      }

      if(newPosition) {
        position = newPosition;
      }
      
      // IMPROVE: Need a more complete algorithm here
      if(position <= 8) {
        // Need to turn sharp left
        powerLeft =  0.0;
        powerRight = 0.35;
      }
      else if(position < 24) {
        // Need to turn left
        powerLeft =  0.20;
        powerRight = 0.35;
      }
      else if(position <= 48) {
        // Continue forwards
        powerLeft = powerRight = 0.25;
      }
      else if(position < 128) {
        // Turn right
        powerLeft = 0.35;
        powerRight = 0.20;
      }
      else {
        // Turn sharp right
        powerLeft = 0.35;
        powerRight = 0.00;
      }

      //std::cout << "Sensor result ";


      //std::cout << " = " << position ;

      //std::cout << std::endl;

      // Set the motors
      if(lastPowerLeft != powerLeft || lastPowerRight != powerRight) {

        robot()->powertrain()->setPower(powerLeft, powerRight);

        lastPowerLeft = powerLeft;
        lastPowerRight = powerRight;
      }
    }
    else {
      std::cerr << __func__ << ": Failed to read in sensor details" << std::endl;
    }
    
    // Let the robot actually move
    //std::this_thread::sleep_for (std::chrono::milliseconds(10));
    
  }

  // Ensure the motors are stopped
  robot()->powertrain()->stop();

  // Release the sensor
  _qtr8rc->disable();
}

}
