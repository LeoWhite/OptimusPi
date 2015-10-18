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

bool ThoughtProcess_Proximity::available() {
  return _vl6180->exists();
}

bool ThoughtProcess_Proximity::prepare() {
  return _vl6180->enable();
}

void ThoughtProcess_Proximity::run() {
  bool running = true;
  float lastPower = -1.0;

  while(running) {
    uint8_t range = _vl6180->range();
    float power = 0.0;

    if(range < 20) {
      std::cerr << "Stopping" << std::endl;
      // Stop!
      power = 0.0;

      // and we've completed
      running = false;
    }
    else if(range < 50) {
      // getting closer
      power = 0.25;
    }
    else if(range < 100) {
      // Drop to quarter
      power = 0.50;
    }
    // Long way to go yet!
    else {
      // Proceed forwards at half speed
      power = 0.66;
    }

    // Set the motors
    if(lastPower != power) {
      robot()->powertrain()->setPower(power, power);
      std::cerr << "Range is " << (int)range << "mm setting power to " << power << std::endl;
    } 

    // Let the robot actually move
    std::this_thread::sleep_for (std::chrono::microseconds(100));
  }
 
  robot()->powertrain()->stop();
}

void ThoughtProcess_Proximity::stop() {
}

}
