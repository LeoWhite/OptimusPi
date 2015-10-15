/**
 * The SensorVL6180 class initialises and controls access to 
 * the SparkFun VL6180 Range sensor.
 * https://cdn.sparkfun.com/assets/learn_tutorials/2/5/7/VL6180_ApplicationNote.pdf
 */

#ifndef _PIWARS_SENSORVL6180_H
#define _PIWARS_SENSORVL6180_H

#include <cstdint>
#include <cstddef>

#include "Sensor.h"
#include "I2C.h"

namespace PiWars {

class SensorVL6180 : public Sensor, public I2C {
  public:
    // Initialize the VL6180 ready for use
    SensorVL6180();
    ~SensorVL6180();

    // Checks if the sensor is present. Some sensors can be dynamically
    // added or removed from the robot, or turned on and off, so arn't
    // always available for use.
    //
    // @returns true if the sensor exists
    bool exists();
    
    uint8_t range();

  private:
    void init();
    void writeByte(uint16_t reg, char data);
    char readByte(uint16_t reg);

    bool _initialised; //<! Indicates if the sensor has been intialised
};

}

#endif