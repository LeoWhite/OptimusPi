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
  float lastPower = -1.0;

  while(true) {
    uint16_t sensorDiff[8] = {0};
    uint16_t position;
    
    if(_qtr8rc->readLine(sensorDiff, position)) {
      std::cout << "Sensor result ";
        
      for(size_t i = 0; i < 8; i++) {
        std::cout << (uint32_t)sensorDiff[i] << ":";
      }
      
     std::cout << " = " << position <<std::endl;
    }
    else {
      std::cerr << "Failed to read in sensor deatsils" << std::endl;
    }
    
    
    
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
 
  robot()->powertrain()->stop();
}

void ThoughtProcess_LineFollower::stop() {
}

}
