/**
 * Represents an InputEvent, a specific input change (e.g. button press) that
 * occurs at a specific time
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
      // Creates an instance of an Input event that holds all the
      // information around a single input event that can
      // be sent through the system.
      //
      // @param code The code for this event (e.g. KEY_A, KEY_ENTER)
      // @param value The value for this event (e.g. pressed, release)
      InputEvent(uint32_t code, int32_t value);
      InputEvent(uint32_t code, float value);
      ~InputEvent() {};

      // Gets the type of this event
      // @returns the type of this event
      InputEventType getType() const { return _type; }

      // Returns the code of this event
      // @returns the code e.g. KEY_A
      uint32_t getCode() const { return _code; }

      // Query the button value (only valid if type == BUTTON)
      // @returns the stored value
      int32_t getButtonValue() const { return _buttonValue; }

      // Query the axis value (only valid if type == AXIS)
      // @returns the stored value in the range -1.0 to 1.0
      float getAxisValue() const { return _axisValue; }

    private:
      InputEventType _type;  //<! The type of this event

      uint32_t _code; //<! The code of this event
      int32_t _buttonValue; //<! The button value (If type == BUTTON)
      float _axisValue; //<! The stored axis value (If type == AXIS)
  };

  // The InputEventQueue is a type of MessageQueue that
  // contains a buffered list of InputEvents in the order they arrived
  class InputEventQueue : public MessageQueue<InputEvent> {
  };

}
#endif
