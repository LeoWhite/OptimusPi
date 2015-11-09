/**
 * PiWars
 *
 * A library of functionality targetted towards performing the PiWars 2015 challenges.
 */
#ifndef _PIWARS_PIWARS_H
#define _PIWARS_PIWARS_H

class ArduiPi_OLED;

// Our PiWars namespace to make sure we don't clash with any other APIs/libraries we
// may end up using
namespace PiWars {

// Forwards declaratinos
class Brains;
class Powertrain;
class InputDevice;
class InputEvent;
class InputEventQueue;

class PiWars {
  public:
    PiWars();

    ~PiWars();

    /**
     * Returns the 'Brains' for this PiWars instance.
     */
    Brains *brains() { return _brains; }

    /**
     * Returns the 'PowerTrain' for this PiWars instance.
     */
    Powertrain *powertrain() { return _powertrain; }

    // The main control loop for the robot, deals with 
    // selecting the process to tun etc.
    void run();

    void processButton(const InputEvent &event);
    void updateDisplay();
    
  private:

    Brains *_brains;
    Powertrain *_powertrain;
    ArduiPi_OLED *_display;
    InputDevice *_fiveWay;
    InputEventQueue *_inputQueue;    
    
    bool _menuActive;
};

}
#endif
