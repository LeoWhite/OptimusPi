/**
 * Represents an InputEvent
 */
#ifndef _PIWARS_INPUTEVENT_H
#define _PIWARS_INPUTEVENT_H

#include <cstdint>
#include <string>
#include "linux/input.h"

#include "MessageQueue.h"

namespace PiWars {
  enum class InputEventType {
    BUTTON,
    AXIS
  };

  enum class InputEventButtonValue {
    RELEASE = 0,
    PRESS,
    REPEAT
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
      InputEvent() {};
      ~InputEvent() {};

      // Returns the type of this event
      InputEventType getType() const { return _type; }
      
      // Returns the code of this event
      uint32_t getCode() const { return _code; }

      // Query the button value
      int32_t getButtonValue() const { return _buttonValue; }
      
      // Query the axis value
      float getAxisValue() const { return _axisValue; }

    private:
      InputEventType _type;
      
      uint32_t _code;
      int32_t _buttonValue;
      float _axisValue;
  };
  
  class InputEventQueue : public MessageQueue<InputEvent> {
  };
  
}
#endif
