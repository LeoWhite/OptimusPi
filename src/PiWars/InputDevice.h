/**
 * Represents the various Input Devices
 *
 * Kernel Input event information https://www.kernel.org/doc/Documentation/input/event-codes.txt
 */
#include <cstdint>
#include <string>
#include <thread>
#include "linux/input.h"

#include "libevdev.h"


namespace PiWars {
  
  enum class InputDeviceType {
    JOYSTICK
  };

  // Represents an input device on the system, processing it
  // for events
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
      
      // Thread function for processing the events
      static void processEvents(struct libevdev *evdev, int processingFD);
      static void handleEvent(struct input_event *event);
      
      
      std::string _inputPath; //!< The file path of the input device
      int         _fd;        //!< File descriptor for the input device
      struct libevdev *_evdev; //!< Used to process events for this device

      std::thread *_eventProcessing; //!< Thread used for processing the input event
      int _eventProcessingFD; //!< Used to pass messages to the event processing thread
        
      std::string _name; //!< The reported name of the device
      std::size_t _numButtons; //!< The total number of buttons on this device
      std::size_t _numAxes; //!< The total numer of axes available on this device
      bool    _claimed; //!< Check if this is claimed for use
  };
}
