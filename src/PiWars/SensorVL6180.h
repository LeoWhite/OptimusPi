/**
 * The SensorVL6180 class initialises and controls access to 
 * the SparkFun VL6180 Range sensor.
 * https://cdn.sparkfun.com/assets/learn_tutorials/2/5/7/VL6180_ApplicationNote.pdf
 */

#ifndef _PIWARS_SENSORVL6180_H
#define _PIWARS_SENSORVL6180_H

#include <atomic>
#include <thread>
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

    // Enables the sensor ready for use.
    // Some sensors use additional power or CPU resources
    // when running, so we only want to enable them when
    // we're ready to use them
    // @returns true if successfully enabled
    bool enable();

    // Disable a sensor, potentially reducing power or CPU usage
    void disable();
   
    // Returns the current range in mm
    //
    // @returns The range in mm.  Note: A range of 255 indicates the range wasn't
    // read for some reason
    uint8_t range() { return _range.load(); }

  private:
    void init(); //<! Initialise the range sensor
    static void writeByte(I2C *i2c, uint16_t reg, char data); //<! The VL6180 has 16 bit registers, so we need a special write call
    static char readByte(I2C *i2c, uint16_t reg); //<! The VL6180 has 16 bit registers, so we need a special read call
    static void rangeReader(std::atomic<bool> &quit, std::atomic<uint8_t> &range); //<! Background thread for polling the sensor

    bool _initialised; //<! Indicates if the sensor has been intialised
    std::atomic<uint8_t> _range; //<! The last successfully read in range.
      
    std::thread *_rangeReader; //<! Background thread for reading in the range
    std::atomic<bool> _rangeReaderQuit; //<! Used to indicate when the thread should exit
};

}

#endif
