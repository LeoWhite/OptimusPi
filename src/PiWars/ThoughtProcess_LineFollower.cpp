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

bool ThoughtProcess_LineFollower::available() {
  return _qtr8rc->exists();
}

bool ThoughtProcess_LineFollower::prepare() {
  return _qtr8rc->enable();
}

void ThoughtProcess_LineFollower::run() {
  bool running = true;
  float lastPowerLeft = -1.0, lastPowerRight = -1.0;

  while(true) {
    uint16_t sensorDiff[8] = {0};
    uint16_t position;
    float powerLeft, powerRight;
    
    if(_qtr8rc->readLine(sensorDiff, position)) {
      std::cout << "Sensor result ";
        
      for(size_t i = 0; i < 8; i++) {
        std::cout << (uint32_t)sensorDiff[i] << ":";
      }
      
      std::cout << " = " << position <<std::endl;
    
#if 0 
      // Are we over the line
      if(position <= 100) {
        // Line is very far to the left.
        powerLeft = 0.20;
        powerRight = 0.60;       
      }
      else if(position <= 2000) {
        // Line is far to the left.
        powerLeft = 0.30;
        powerRight = 0.50;       
      }
      else if(position <= 3000) {
        // Line is slightly to the left.
        powerLeft = 0.30;
        powerRight = 0.40;       
      }
      else if(position <= 4000) {
        // Line is under the sensor
        powerLeft = 0.30;
        powerRight = 0.30;       
      }
      else if(position <= 5000) {
        // Line is slightly to the right
        powerLeft = 0.40;
        powerRight = 0.30;              
      }
      else if(position <= 6000) {
        // Line is far to the right
        powerLeft = 0.50;
        powerRight = 0.30;       
      }
      else  {
        // Line is very far to the right
        powerLeft = 0.60;
        powerRight = 0.20;       
      }
#endif

      // Very simple implemenation
      if(position <= 3000) {
        // Need to turn left
        powerLeft = -0.35;
        powerRight = 0.35;
      }
      else if(position < 4000) {
        // Continue forwards
        powerLeft = powerRight = 0.25;
      }
      else {
        // Turn right
        powerLeft = 0.35;
        powerRight = -0.35;
      }
      
      // Set the motors
      if(lastPowerLeft != powerLeft || lastPowerRight != powerRight) {
        float temp = powerLeft;

        // As we are going backwards... swap the values around
        powerLeft = -powerRight;
        powerRight = -temp; 
        robot()->powertrain()->setPower(powerLeft, powerRight);
        std::cerr << "setting power to " << powerLeft << ":" << powerRight << std::endl;
        lastPowerLeft = powerLeft;
        lastPowerRight = powerRight;
      } 

      // Let the robot actually move
      std::this_thread::sleep_for (std::chrono::microseconds(200));
      
    }
    else {
      std::cerr << "Failed to read in sensor deatsils" << std::endl;
    }
  }
 
  robot()->powertrain()->stop();
}

void ThoughtProcess_LineFollower::stop() {
}

}
