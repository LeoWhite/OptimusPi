/**
 * This Sensor interfaces with the RTIMU library to read
 * in sensor details from the connected SenseHAT
 */

#ifndef _PIWARS_SENSORRTIMU_H
#define _PIWARS_SENSORRTIMU_H

#include <atomic>
#include <thread>
#include <cstdint>
#include <cstddef>

#include "Sensor.h"
#include "I2C.h"

namespace PiWars {

class SensorRTIMU : public Sensor {
  public:
    // Initialize the RTIMU ready for use
    SensorRTIMU();
    ~SensorRTIMU();

    // Checks if the sensor is present. Some sensors can be dynamically
    // added or removed from the robot, or turned on and off, so arn't
    // always available for use.
    //
    // @returns true if the sensor exists
    bool exists();

    // Enables the sensor ready for use.
    // This starts a background thread to constantly read in and update
    // the sensor results
    //
    // @returns true if successfully enabled
    bool enable();

    // Disable the sensor, requesting the background thread
    // to exit and waiting for it to finish
    void disable();

    // Returns the current values
    //
    // @param pitch Filled in with the current pitch
    // @param roll  Filled in with the current roll
    // @param yaw   Filled in with the current yaw
    void fusion(float &pitch, float &roll, float &yaw);

  private:
    void init(); //<! Initialise the range sensor
    static void rtimuReader(std::atomic<bool> &quit, std::atomic<float> &pitch, std::atomic<float> &roll, std::atomic<float> &yaw); //<! Background thread for polling the sensor

    bool _initialised; //<! Indicates if the sensor has been intialised
    std::atomic<float> _pitch; //<! The last successfully read in pitch.
    std::atomic<float> _roll; //<! The last successfully read in roll.
    std::atomic<float> _yaw; //<! The last successfully read in yaw.

    std::thread *_rtimuReader; //<! Background thread for reading in the values
    std::atomic<bool> _rtimuReaderQuit; //<! Used to indicate when the thread should exit
};

}

#endif
