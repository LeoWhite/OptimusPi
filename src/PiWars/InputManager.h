/**
 * Deals with the various input mechanism that we will
 * be utilizing (mostly just the joystick to begin with
 */
#ifndef _PIWARS_INPUTMANAGER_H
#define _PIWARS_INPUTMANAGER_H

namespace PiWars {

  class InputDevice;

  // The InputManager maintains a list of all currently available
  // Input devices detected, updating it dynamically as devices are
  // added or removed
  //
  class InputManager {
    public:
      // Not currently IMPLEMENTED!
      InputManager();
      ~InputManager();
  };
}

#endif
