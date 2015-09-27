#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "InputDevice.h"

namespace PiWars
{

InputDevice::InputDevice(std::string &filePath) : _inputPath(filePath), _fd(-1), _evdev(nullptr), _numButtons(0), _numAxes(0), _claimed(false)
{
  if(claim()) {
    populateInfo();
    release();
  }
}

InputDevice::~InputDevice()
{
}

bool InputDevice::claim() {
  // If the device is already claimed, then don't claim it again.
  if(_claimed) {
    return _claimed;
  }

  // Attempt to open the input device
  _fd = open(_inputPath.c_str(), O_RDONLY);
    
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

  // close out the evdev structure
  libevdev_free(_evdev);
  _evdev = nullptr;
  
  // Its now safe to release the file descriptor
  close(_fd);
  _fd = -1;
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
}
