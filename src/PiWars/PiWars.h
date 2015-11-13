/**
 * PiWars
 *
 * A library of functionality targetted towards performing the PiWars 2015 challenges.
 */
#ifndef _PIWARS_PIWARS_H
#define _PIWARS_PIWARS_H

#include <chrono>

// Forward declared classes
class ArduiPi_OLED;

// Our PiWars namespace to make sure we don't clash with any other APIs/libraries we
// may end up using
namespace PiWars {

// Forwards declarations of PiWar classes
class Brains;
class Powertrain;
class InputDevice;
class InputEvent;
class InputEventQueue;
class Menu;

class PiWars {
  public:
    PiWars();

    ~PiWars();

    // Returns the 'Brains' for this PiWars instance.
    //
    // @returns the Brains object
    Brains *brains() { return _brains; }

    // Returns the 'PowerTrain' for this PiWars instance.
    //
    // @returns The Powertrain object
    Powertrain *powertrain() { return _powertrain; }

    // The main control loop for the robot, deals with
    // selecting the process to run, display menus etc.
    void run();

    // Handles a button press
    //
    // @param event The InputEvent to process
    void processButton(const InputEvent &event);

    // Updates the OLED display with the current information
    void updateDisplay();

  private:
    bool _running; //<! Are we still running?
    Brains *_brains; //<! The 'Brains' of this robot
    Powertrain *_powertrain; //<! The 'PowerTrain' of this robot
    ArduiPi_OLED *_display; //<! The connected OLED display
    InputDevice *_fiveWay; //<! The Fizeway controller on the SenseHAT
    InputEventQueue *_inputQueue; //<! The queue of InputEvents

    std::chrono::time_point<std::chrono::system_clock> _lastInput; //<! When we last processed an Input Event
    Menu *_mainMenu; //<! The PiWars main menu for display
    Menu *_currentMenu; //<! The menu that is currently being displayed/accesed
};

}
#endif
