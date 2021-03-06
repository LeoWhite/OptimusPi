/**
 * Represents the various Input Devices
 *
 * Kernel Input event information https://www.kernel.org/doc/Documentation/input/event-codes.txt
 */

#ifndef _PIWARS_INPUTDEVICE_H
#define _PIWARS_INPUTDEVICE_H

#include <cstdint>
#include <string>
#include <thread>
#include "linux/input.h"

#include "libevdev.h"


namespace PiWars {
  // Forward declarations of classes
  class InputEvent;
  class InputEventQueue;

  enum class InputDeviceType {
    JOYSTICK
  };

  // Represents an input device on the system, processing it
  // for events
  class InputDevice {
    public:
      // Creates a class that represents the specific input device on 
      // the system, processing it for events.
      // @param inputPath The input device this object represents
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

      // Sets the InputEvent queue to send input to
      // IMPROVE: Eventually multiple events queues will be registered,
      // i.e. on to monitor the axes, another to listen for a specific button press
      void setEventQueue(InputEventQueue &queue);

      // Removes the assigned event queue
      void resetEventQueue();

    private:
      // Reads in, and caches, information about the input device
      void populateInfo();

      // Thread function for processing the events
      static void processEvents(struct libevdev *evdev, int processingFD, InputEventQueue *queue);
      static void handleEvent(struct input_event *event, InputEventQueue *queue);


      std::string _inputPath; //!< The file path of the input device
      int         _fd;        //!< File descriptor for the input device
      struct libevdev *_evdev; //!< Used to process events for this device

      std::thread *_eventProcessing; //!< Thread used for processing the input event
      int _eventProcessingFD; //!< Used to pass messages to the event processing thread

      std::string _name; //!< The reported name of the device
      std::size_t _numButtons; //!< The total number of buttons on this device
      std::size_t _numAxes; //!< The total numer of axes available on this device
      bool    _claimed; //!< Check if this is claimed for use

      InputEventQueue *_queue; //!< Input queue to send events to. Liable to change.. for test use only
  };
}
#endif
