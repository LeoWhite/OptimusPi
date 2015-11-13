#include "SensorRTIMU.h"
#include "RTIMULib.h"

#include <iostream>
#include <thread>

namespace PiWars
{


SensorRTIMU::SensorRTIMU()
  : Sensor()
  , _initialised(false)
  , _pitch(0)
  , _roll(0)
  , _yaw(0)
  , _rtimuReader(nullptr)
  , _rtimuReaderQuit(false)
{
}

SensorRTIMU::~SensorRTIMU() {
}

bool SensorRTIMU::exists() {
  // Always return true for now, as the RTIMU library supports various
  // sensors
  return true;
}

bool SensorRTIMU::enable() {
  if(!isEnabled()) {
    // Create a thread to poll the range sensor, so there is always a valid
    // range ready to be read.
    _rtimuReaderQuit = false;
    _rtimuReader = new std::thread(rtimuReader, std::ref(_rtimuReaderQuit), std::ref(_pitch), std::ref(_roll), std::ref(_yaw));

    // Call the base class to perform any
    // generic changes
    Sensor::enable();
  }

  return isEnabled();

}

void SensorRTIMU::disable() {
  if(isEnabled()) {
    // Tell the thread to exit
    _rtimuReaderQuit = true;

    // and wait for it to do so
    _rtimuReader->join();
    _rtimuReader = nullptr;

    Sensor::disable();
  }
}
void SensorRTIMU::fusion(float &pitch, float &roll, float &yaw) {
  pitch = _pitch;
  roll = _roll;
  yaw = _yaw;
}

void SensorRTIMU::rtimuReader(std::atomic<bool> &quit, std::atomic<float> &pitch, std::atomic<float> &roll, std::atomic<float> &yaw)
{
  // read in the main settings and create the RTIMU class
  RTIMUSettings *settings = new RTIMUSettings("/etc", "RTIMULib");
  RTIMU *imu = RTIMU::createIMU(settings);

  if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
    std::cerr << __func__ << ": IMU not found!" << std::endl;

    // Exit out early
    return;
  }

  imu->IMUInit();

  // Use the recommended values
  imu->setSlerpPower(0.02);
  imu->setGyroEnable(true);
  imu->setAccelEnable(true);
  imu->setCompassEnable(true);

  while(!quit.load()) {

    std::this_thread::sleep_for (std::chrono::milliseconds(imu->IMUGetPollInterval()));

    if(imu->IMURead()) {
      RTIMU_DATA imuData = imu->getIMUData();

      if(imuData.fusionPoseValid) {
        pitch = imuData.fusionPose.x()* RTMATH_RAD_TO_DEGREE;
        roll = imuData.fusionPose.y()* RTMATH_RAD_TO_DEGREE;
        yaw = imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE;
      }
    }
  }

  delete imu;
  delete settings;
}

void SensorRTIMU::init() {
  // Nothing to be done for now
}

}
