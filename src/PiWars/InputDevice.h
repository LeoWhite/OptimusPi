/**
 * Represents the various Input Devices
 */
#include <cstdint>
#include <string>

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
      std::string &getName();

      // Returns the number of axis on this device
      uint8_t getNumAxis();

      // Returns the number of buttons on this device
      uint8_t getNumButtons();

    private:
      std::string _inputPath; //!< The file path of the input device
      int         _fd;        //!< File descriptor for the input device
      struct libevdev *_evdev; //!< Used to process events for this device

      uint8_t _numButtons; //!< The total number of buttons on this device
      uint8_t _numAxis; //!< The total numer of axis available on this device
      bool    _claimed; //!< Check if this is claimed for use
  };
}
