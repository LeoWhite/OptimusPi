/**
 * The SensorQTR8RC class initialises and controls access to 
 * the SparkFun VL6180 Range sensor.
 * https://cdn.sparkfun.com/assets/learn_tutorials/2/5/7/VL6180_ApplicationNote.pdf
 */

#ifndef _PIWARS_SENSORQTR8RC_H
#define _PIWARS_SENSORQTR8RC_H

#include <atomic>
#include <thread>
#include <cstdint>
#include <cstddef>

#include "Sensor.h"
#include "I2C.h"

namespace PiWars {

class SensorQTR8RC : public Sensor, public I2CInternal {
  public:
    // Initialize the VL6180 ready for use
    SensorQTR8RC();
    ~SensorQTR8RC();

    // Checks if the sensor is present. Some sensors can be dynamically
    // added or removed from the robot, or turned on and off, so arn't
    // always available for use.
    //
    // @returns true if the sensor exists
    bool exists();

    // Enables the sensor ready for use.
    // Some sensors use additional power or CPU resources
    // when running, so we only want to enable them when
    // we're ready to use them
    // @returns true if successfully enabled
    bool enable();

    // Disable a sensor, potentially reducing power or CPU usage
    void disable();
   
    // Returns the current sensor reading and position
    //

    bool readLine(uint16_t (&sensorDiff)[8], uint16_t &position);

  private:
    void init(); //<! Initialise the range sensor

    bool _initialised; //<! Indicates if the sensor has been intialised
};

}

#endif
