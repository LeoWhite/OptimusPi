#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <linux/input.h>

#include <poll.h>
#include <sys/eventfd.h>
#include "InputDevice.h"
#include "InputEvent.h"

#include <iostream>

namespace PiWars
{

InputDevice::InputDevice(std::string &filePath) :
  _inputPath(filePath), _fd(-1), _evdev(nullptr), _eventProcessing(nullptr),
  _eventProcessingFD(-1),  _numButtons(0), _numAxes(0), _claimed(false), _queue(nullptr)
{
  _eventProcessingFD = eventfd(0, 0);

  // IMPROVE: This will cause an event processing thread to
  // be created and destroyed. Ideally we want to skip that state
  // during initialisation
  if(claim()) {
    populateInfo();
    release();
  }
}

InputDevice::~InputDevice()
{
  // Ensure that the device has been release
  release();

  // Close the event FD
  if(-1 != _eventProcessingFD) {
    close(_eventProcessingFD);
  }
}

bool InputDevice::claim() {
  // If the device is already claimed, then don't claim it again.
  if(_claimed) {
    return _claimed;
  }

  // Attempt to open the input device
  _fd = open(_inputPath.c_str(), O_RDONLY|O_NONBLOCK);

  if(_fd) {
    int result;

    // Connect the device to the evdev library
    result = libevdev_new_from_fd(_fd, &_evdev);

    if(0 != result) {
      _evdev = nullptr;
      close(_fd);
      _fd = -1;
      return false;
    }

    // Create a FD to pass messages to the processing thread
    // Spin off a thread to handle processing the input
    _eventProcessing = new std::thread(processEvents, _evdev, _eventProcessingFD, _queue);

    // Successfully claimed!
    _claimed = true;
  }

  return _claimed;
}


void InputDevice::release() {
  // If the device isn't claimed, then nothing to do
  if(!_claimed) {
    return;
  }

  // Tell the processing thread to exit
  uint64_t value = 1;
  write(_eventProcessingFD, &value, sizeof(value));

  // and wait for it to do so
  _eventProcessing->join();
  _eventProcessing = nullptr;

  // close out the evdev structure
  libevdev_free(_evdev);
  _evdev = nullptr;

  // Its now safe to release the file descriptor
  close(_fd);
  _fd = -1;

  // and we're no longer 'claimed'
  _claimed = false;
}

void InputDevice::populateInfo(void) {
  // Read in the name
  _name = libevdev_get_name(_evdev);

  // Count the number of axes present
  for(std::size_t i = 0; i < ABS_MAX; i++) {
    if(libevdev_has_event_code(_evdev, EV_ABS, i)) {
      _numAxes++;
    }
  }

  // and count the number of buttons
  for(std::size_t i = 0; i < KEY_MAX; i++) {
    if(libevdev_has_event_code(_evdev, EV_KEY, i)) {
      _numButtons++;
    }
  }

}

void InputDevice::setEventQueue(InputEventQueue &queue) {
  _queue = &queue;
}

void InputDevice::resetEventQueue() {
  _queue = nullptr;
}

void InputDevice::processEvents(struct libevdev *evdev, int processingFD, InputEventQueue *queue) {
  struct pollfd fds[2];

  // Query the file descriptor so we can correctly wait for events
  fds[0].fd = libevdev_get_fd(evdev);
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  // and we also want to be woken up when this device
  // is released.
  fds[1].fd = processingFD;
  fds[1].events = POLLIN;
  fds[1].revents = 0;


  while(int result = poll(fds, 2, -1)) {
    // Something went wrong (FD got closed, device was removed)
    // so we simply exit out
    if(-1 == result) {
      std::cerr << "Poll returned error" << std::endl;
      return;
    }
    else {
      // Have we been told to exit?
      if(fds[1].revents & POLLIN) {
        uint64_t value;

        // Read and discard the value to clear it
        read(fds[1].fd, &value, sizeof(value));

        // and exit
        return;
      }
      // Anyting else should be from the input device
      else if(fds[0].revents & POLLIN) {
        int rc;
        struct input_event ev;

        // Tell poll that we have processed the event
        fds[0].revents = 0;

        // Clear out any queued up input events
        do {
          rc = libevdev_next_event(evdev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

          // We're dropping events!
    			if(LIBEVDEV_READ_STATUS_SYNC == rc) {
    			  // IMPROVE: we should re-sync here
    				std::cout <<  "Error: cannot keep up\n" << std::endl;
    			}
    			// An error occured when reading from the FD, maybe it was closed
    			// or the device removed
    			else if (rc != -EAGAIN && rc < 0) {
    				std::cout << "Error: " << (-rc) << std::endl;
    				return;
    			}
    			// We've actually read something!
    			else if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
    			  handleEvent(&ev, queue);
    			}
        } while (rc != -EAGAIN);
      }
      else {
        std::cout << "Unknown event" << std::endl;
      }
    }
  }
}

void InputDevice::handleEvent(struct input_event *event, InputEventQueue *queue) {
  InputEvent *inputEvent = nullptr;
  
  // Is it a key code?
  if(libevdev_event_is_type(event, EV_KEY)) {
    inputEvent = new InputEvent(event->code, event->value);
  }
  else if(libevdev_event_is_type(event, EV_ABS)) {
    // Normalise the axis into the range -1 to 1
    float axisValue = (-1.0 + (2.0/255.0)* event->value);
    
    inputEvent = new InputEvent(event->code, axisValue);
  }
  
  // Did we get an input event?
  if(nullptr != inputEvent && nullptr != queue) {
    queue->push(*inputEvent);
  }
}
  

}
