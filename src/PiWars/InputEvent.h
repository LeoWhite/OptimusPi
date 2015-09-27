/**
 * Represents an InputEvent
 */
#include <cstdint>
#include <string>
#include "linux/input.h"

namespace PiWars {
  enum class InputEventType {
    BUTTON,
    AXIS
  };

  // Holds all the information around an InputEvent that can
  // be sent through the system
  class InputEvent {
    public:
      /**
       * inputPath - The input device this object represents
       */
      InputEvent(uint32_t code, int32_t value);
      InputEvent(uint32_t code, float value);
      ~InputEvent();

      // Returns the type of this event
      InputEventType getType() { return _type; }
      
      // Returns the type of this event
      uint32_t getCode() { return _code; }

      // Query the button value
      int32_t getButtonValue() { return _buttonValue; }
      
      // Query the axis value
      float getAxisValue() { return _axisValue; }

    private:
      InputEventType _type;
      
      uint32_t _code;
      int32_t _buttonValue;
      float _axisValue;
  };
}
