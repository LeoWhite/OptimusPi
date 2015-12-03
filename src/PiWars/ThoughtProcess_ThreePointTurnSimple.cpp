/**
 * This ThoughtProcess is responsible for guiding the robot through
 * the three point turn challenge
 *
 * http://piwars.org/2015-competition/challenges/three-point-turn/
 *
 * This is the 'simple' implementation that uses dead-reckoning to
 * complete the course
 */

#include <cstdint>
#include <cstddef>
#include <unistd.h>
#include <thread>

#include "ThoughtProcess.h"
#include "ThoughtProcess_ThreePointTurnSimple.h"
#include "PiWars.h"
#include "Powertrain.h"
#include <iostream>

namespace PiWars {


ThoughtProcess_ThreePointTurnSimple::ThoughtProcess_ThreePointTurnSimple(PiWars *robot) : ThoughtProcess(robot) {
}

ThoughtProcess_ThreePointTurnSimple::~ThoughtProcess_ThreePointTurnSimple() {
}

const std::string &ThoughtProcess_ThreePointTurnSimple::name() {
  static std::string name("ThreePointTurnSimple");

  return name;
}

bool ThoughtProcess_ThreePointTurnSimple::available() {
  return true;
}

bool ThoughtProcess_ThreePointTurnSimple::prepare() {
  return true;
}

void ThoughtProcess_ThreePointTurnSimple::run(std::atomic<bool> &running) {
  std::chrono::time_point<std::chrono::system_clock> start, end;

  // Drive forwards
  driveForDuration(running, false, 7.5);

  // Turn left
  turnLeft(running);

  // Drive forwards again
  driveForDuration(running, false, 1.4f);
  
  // Drive backwards
  driveForDuration(running, true, 3.0f);
  
  // Forwards again
  driveForDuration(running, false, 1.5f);

  // Turn left again
  turnLeft(running);
  
  // and finally head home
  driveForDuration(running, false, 7.5f);
}

void ThoughtProcess_ThreePointTurnSimple::driveForDuration(std::atomic<bool> &running, const bool backwards, const float seconds)
{
  std::chrono::time_point<std::chrono::system_clock> start, end;
  float lastPowerLeft = 0.0f, lastPowerRight = 0.0f;

  // Note the start time
  start = std::chrono::system_clock::now();

  // Move forwards on the current heading
  while(running.load()) {
    float powerLeft, powerRight;

    // Drive at half speed
    powerLeft = 0.50;
    powerRight = 0.50;

    // Set the motors
    //if(lastPowerLeft != powerLeft || lastPowerRight != powerRight) 
    {
      // Are we moving backwards?
      if(backwards) {
        float temp = powerLeft;

        powerLeft = -powerRight;
        powerRight = -temp;
      }

      // The robot has a slight drift, so correct here
      powerRight = powerLeft * 0.95;
      
      if(robot()->powertrain()->setPower(powerLeft, powerRight)) {
        lastPowerLeft = powerLeft;
        lastPowerRight = powerRight;
      }
    }

    // Let the robot actually move
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    end = std::chrono::system_clock::now();
    std::chrono::duration<float> duration = end - start;

    if(duration.count() >= seconds){
      break;
    }
  }

  // and stop
  robot()->powertrain()->stop();
}

void ThoughtProcess_ThreePointTurnSimple::turnLeft(std::atomic<bool> &running) {
  std::chrono::time_point<std::chrono::system_clock> start, end;

  // Note the start time
  start = std::chrono::system_clock::now();

  // Start turning left
  robot()->powertrain()->setPower(-0.50, 0.50);
  while(running.load())
  {
    // Keep turning left
    robot()->powertrain()->setPower(-0.50, 0.50);

    // Let the robot actually move
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
    
    end = std::chrono::system_clock::now();
    std::chrono::duration<float> duration = end - start;

    // We want to turn 90 degrees
    if(duration.count() >= 1.45f){
      break;
    }
  }

  // Stop
  robot()->powertrain()->stop();
}

}

