/**
 * Represents the various Input Devices
 */
#include <cstdint>
#include <string>
#include "linux/input.h"


#include "libevdev.h"

namespace PiWars {
  enum class InputDeviceType {
    JOYSTICK
  };

  class InputDevice {
    public:
      /**
       * inputPath - The input device this object represents
       */
      InputDevice(std::string &inputPath);
      ~InputDevice();

      // Claim the device for use
      bool claim();

      // Releases the device
      void release();

      // Returns the device type
      uint8_t getType();

      // Returns the device name
      std::string &getName() { return _name; }

      // Returns the number of axes on this device
      std::size_t getNumAxes() { return _numAxes; }

      // Returns the number of buttons on this device
      std::size_t getNumButtons() { return _numButtons; }

    private:
      // Reads in, and cache, information about the input device
      void populateInfo();
      
      std::string _inputPath; //!< The file path of the input device
      int         _fd;        //!< File descriptor for the input device
      struct libevdev *_evdev; //!< Used to process events for this device

      std::string _name; //!< The reported name of the device
      std::size_t _numButtons; //!< The total number of buttons on this device
      std::size_t _numAxes; //!< The total numer of axes available on this device
      bool    _claimed; //!< Check if this is claimed for use
  };
}
