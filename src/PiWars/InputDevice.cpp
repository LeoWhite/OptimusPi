#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "InputDevice.h"

namespace PiWars
{

InputDevice::InputDevice(std::string &filePath) : _inputPath(filePath), _fd(-1), _evdev(nullptr), _numButtons(0), _numAxis(0), _claimed(false)
{
}

InputDevice::~InputDevice()
{
}

bool InputDevice::claim() {
  // If the device is already claimed, then don't claim it again.
  if(_claimed) {
    return false;
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


  }
  return _claimed;
}
}
