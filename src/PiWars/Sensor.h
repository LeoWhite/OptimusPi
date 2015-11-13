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
    Sensor() : _enabled(false) {
      _fd =  eventfd(0, EFD_NONBLOCK);
    }
    ~Sensor() {
      close(_fd);
    }

    // Returns the FD that threads can block on when waiting
    // for sensor data to change.
    //
    // @returns The File descriptor a thread can block on
    int getFD() { return _fd; };

    // Returns if the sensor has been enabled
    //
    // @returns True is the sensor has been enabled
    //          false otherwise
    bool isEnabled() { return _enabled; }

    // Checks if the sensor is present. Some sensors can be dynamically
    // added or removed from the robot, or turned on and off, so arn't
    // always available for use.
    //
    // @returns true if the sensor exists
    virtual bool exists() = 0;

    // Enables the sensor ready for use.
    // Some sensors use additional power or CPU resources
    // when running, so we only want to enable them when
    // we're ready to use them
    //
    // @returns true if successfully enabled
    bool enable() { _enabled = true; return true; }

    // Disable a sensor, potentially reducing power or CPU usage
    void disable() { _enabled = false; }

  protected:
    // Called when the sensor data is changed, allowing any sleeping threads
    // to be awoken
    void changed() {
      uint64_t value = 1;
      write(_fd, &value, sizeof(value));
    }

  private:
    bool _enabled; //<! Used to indicate if a sensor is currently enabled
    int _fd; //<! The eventfd file descriptor
};

}

#endif
