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

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>

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
  , _displayingInfo(false)
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

      // Do we need to dismiss the menu or Info?
      std::chrono::duration<float> duration = std::chrono::system_clock::now() - _lastInput;

      // After 5 seconds of inactivity we dismiss the menu
      if(duration.count() >= 5.0f) {

        // Ensure the meuu and info are dismissed
        _currentMenu = nullptr;
        _displayingInfo = false;
      }

      // And update the display
      updateDisplay();
    }
    else {
      // Anything else should be from the input device
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
        _displayingInfo = false;
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

        // Assume the 'info' window is no longer being displayed
        _displayingInfo = false;

        // Are we on the main menu?
        if(isMainMenu) {
          if(0 == currentEntry.compare(menuItemInfo)) {
            _displayingInfo = true;
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
  // Are we displaying the info panel?
  if(_displayingInfo) {
    displayInfo();
    return;
  }

  _display->clearDisplay();

  // Setup the Text size and colour incase its been changed
  _display->setTextSize(1);
  _display->setTextColor(WHITE);

  // Draw the first line
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

void PiWars::displayInfo() {
  _display->clearDisplay();

  // Setup the Text size and colour incase its been changed
  _display->setTextSize(1);
  _display->setTextColor(WHITE);

  // Start from the top left
  _display->setCursor(0,0);

  // Display the IP addresses (if any)
  struct ifaddrs *ifaddr;

  if(getifaddrs(&ifaddr) != -1) {
    static std::string eth0("eth0"), wlan0("wlan0");
    char host[NI_MAXHOST];
    struct ifaddrs *ifa;

    /* Walk through linked list, maintaining head pointer so we
       can free list later */
    for(ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
      if(ifa->ifa_addr == NULL) {
        continue;
      }

      // Is it an interface we are interested in?
      if(0 == eth0.compare(ifa->ifa_name) ||
         0 == wlan0.compare(ifa->ifa_name))
      {
        if(ifa->ifa_addr->sa_family == AF_INET) {
           int s;

           // read in the name infdormation
           s = getnameinfo(ifa->ifa_addr,
                    sizeof(struct sockaddr_in),
                   host, NI_MAXHOST,
                   NULL, 0, NI_NUMERICHOST);
           if(0 == s) {
             _display->print(ifa->ifa_name);
             _display->print(":");
             _display->print(host);
             _display->print("\n");
           }
        }
      }
    }
    
    freeifaddrs(ifaddr);
  }

  // and update the actual display
  _display->display();
}
}
