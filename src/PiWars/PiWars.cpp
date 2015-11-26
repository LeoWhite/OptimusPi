/**
 * PiWars
 *
 * A library of functionality targetted towards performing the PiWars 2015 challenges.
 */

#include "PiWars.h"
#include "Brains.h"
#include "Menu.h"
#include "Powertrain.h"
#include "InputDevice.h"
#include "InputEvent.h"
#include <iostream>
#include <sys/poll.h>
#include <sys/stat.h>


// Include last as it defines 'max' and 'min' macros which can break
// the standard C++ includes
#include "ArduiPi_OLED_lib.h"
#include "ArduiPi_OLED.h"

namespace PiWars {

// Static strings for the input and menu
static std::string fiveWayPath = "/dev/input/event0";
static std::string menuItemInfo = "Info";
static std::string menuItemBrains = "Brains";
static std::string menuItemShutdown = "Shutdown";

PiWars::PiWars()
  : _running(false)
  , _brains(new Brains())
  , _powertrain(new Powertrain())
  , _display(new ArduiPi_OLED())
  , _fiveWay(nullptr)
  , _inputQueue(nullptr)
  , _mainMenu(new Menu())
  , _currentMenu(nullptr)
{
  // Ensure the motors are stopped
  _powertrain->stop();

  // Claim the five way controller
  _fiveWay = new InputDevice(fiveWayPath);
  _inputQueue = new InputEventQueue();

  // Attach the input event queue
  _fiveWay->setEventQueue(*_inputQueue);

  if(!_fiveWay->claim()) {
    std::cerr << __func__ << ":Unable to find five way controller" << std::endl;
    exit(-1);
  }

  // Populate the menu
  _mainMenu->add(new MenuItem(menuItemInfo));
  _mainMenu->add(new MenuItem(menuItemBrains));
  _mainMenu->add(new MenuItem(menuItemShutdown));

  // Initialise the display
  _display->init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64);
  _display->begin();

  // Display the initial display state
  updateDisplay();
}

PiWars::~PiWars() {
  // Display shutdown message, so the User knows its being processed
  _display->clearDisplay();
  _display->setTextSize(1);
  _display->setTextColor(WHITE);
  _display->setCursor(0,0);
  _display->print("Shutting down\n");
  _display->display();

  // TODO: Either perform a shutdown here, or in a wrapper script (Makes
  // development hard if we actually turn of the Pi every time we run this!
  delete _mainMenu;
  delete _fiveWay;
  delete _inputQueue;
  delete _brains;
  delete _powertrain;
  delete _display;
}

void PiWars::run() {
  struct pollfd fds[2];

  // We are now running
  _running = true;

  // Query the file descriptor for the InputDevice so we can correctly wait for events
  fds[0].fd = _inputQueue->getFD();
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  while(_running) {
    // Wait for up to a second for input
    int result = poll(fds, 1, 1000);

    if(-1 == result) {
      std::cerr << __func__ << ": Poll returned error, ignoring" << std::endl;
      continue;
    }
    else if(0 == result) {
      // A timeout occured.

      // Do we need to dismiss the menu?
      if(_currentMenu) {
        std::chrono::duration<float> duration = std::chrono::system_clock::now() - _lastInput;

        // After 5 seconds of inactivity we dismiss the menu
        if(duration.count() >= 5.0f) {
          _currentMenu = nullptr;
        }
      }

      // And update the display
      updateDisplay();
    }
    else {
      // Anyting else should be from the input device
      if(fds[0].revents & POLLIN) {
        int rc;
        InputEvent event(0,0);

        // Tell poll that we have processed the event
        fds[0].revents = 0;

        // Process all queued up input events
        while(!_inputQueue->empty()) {
          _inputQueue->pop(event);

          // We're only interested in button presses
          if(InputEventType::BUTTON == event.getType()) {
            processButton(event);
          }
        }
      }
    }
  }
}

void PiWars::processButton(const InputEvent &event) {
  // Remember when we recieved this input
  _lastInput = std::chrono::system_clock::now();

  // We only respond to key release
  if((int32_t)InputEventButtonValue::RELEASE == event.getButtonValue())
  {
    // Is there on menu open?
    if(!_currentMenu) {
      // Is it the 'OK' button
      if(KEY_ENTER == event.getCode()) {
        // Display the main menu
        _currentMenu = _mainMenu;
      }
    }
    else {
      if(KEY_RIGHT == event.getCode()) {
        _currentMenu->next();
      }
      else if(KEY_LEFT == event.getCode()) {
        _currentMenu->previous();
      }
      else if(KEY_ENTER) {
        bool isMainMenu = (_currentMenu == _mainMenu);
        std::string currentEntry = _currentMenu->current();

        // Assume the menu will no longer be active after this
        _currentMenu = nullptr;

        // Are we on the main menu?
        if(isMainMenu) {
          if(0 == currentEntry.compare(menuItemInfo)) {
            // TODO: Display IP address, uptime, date etc.
          }
          else if(0 == currentEntry.compare(menuItemBrains)) {
            // Get the Brains menu and set it as the current menu
            _currentMenu = _brains->menu();
          }
          else if(0 == currentEntry.compare(menuItemShutdown)) {
            _running = false;
          }
        }
        else {
          // Currently we assume it must be the 'Brains' menu. So pass on
          // the select for processing
          _brains->selectMenuEntry(currentEntry);
        }
      }
    }

    // Update the display to reflect any changes above
    updateDisplay();
  }
}

void PiWars::updateDisplay() {
  _display->clearDisplay();

  // Setup the Text size and colour incase its been changed
  _display->setTextSize(1);
  _display->setTextColor(WHITE);

  // Daw the first line
  _display->setCursor(0,0);
  // TODO: Actually display info such as WiFi, BlueTooth, battery level
  _display->print("Stats\n");

  // Display the currently selected process
  _display->setCursor(0,10);
  _display->print("Current:");
  _display->print(_brains->currentThoughtProcess().c_str());

  // If a menu is open, then render it
  if(_currentMenu) {
    _currentMenu->render(_display, 40, 128);
  }

  // and update the actual display
  _display->display();
}

}
