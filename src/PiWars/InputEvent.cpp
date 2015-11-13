#include "InputEvent.h"

#include <iostream>

namespace PiWars
{

InputEvent::InputEvent(uint32_t code, int32_t value) :
  _type(InputEventType::BUTTON), _code(code), _buttonValue(value), _axisValue(0.0f)
{

}

InputEvent::InputEvent(uint32_t code, float value) :
  _type(InputEventType::AXIS), _code(code), _buttonValue(0), _axisValue(value)
{

}

}

