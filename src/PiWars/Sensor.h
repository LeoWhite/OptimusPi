/**
 * The Sensor class represents a variety of sensors that can be attached
 * to a robot, allowing for a base set of APIs that make them
 * easier to use.
 */

#ifndef _PIWARS_SENSOR_H
#define _PIWARS_SENSOR_H

#include <cstdint>
#include <cstddef>
#include <unistd.h>
#include <sys/eventfd.h>

namespace PiWars {

class Sensor {
  public:
    // Initialize the base Sensor class
    Sensor() {
      _fd =  eventfd(0, EFD_NONBLOCK);
    }
    ~Sensor() {
      close(_fd);
    }

    // Checks if the sensor is present. Some sensors can be dynamically
    // added or removed from the robot, or turned on and off, so arn't
    // always available for use.
    //
    // @returns true if the sensor exists
    virtual bool exists() = 0;
    
    // Returns the FD that threads can block on when waiting
    // for sensor data to change.
    int getFD() { return _fd; };

  protected:
    // Called when the sensor data is changed, allowing any sleeping threads
    // to be awoken
    void changed() {
      uint64_t value = 1;
      write(_fd, &value, sizeof(value));      
    }
    
  private:
    int _fd;
};

}

#endif