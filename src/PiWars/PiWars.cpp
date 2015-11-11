#include "PiWars.h"
#include "Brains.h"
#include "Menu.h"
#include "Powertrain.h"
#include "InputDevice.h"
#include "InputEvent.h"
#include <iostream>
#include <sys/poll.h>
#include <sys/stat.h>


// Include last as it defines 'max' and 'min' macros
#include "ArduiPi_OLED_lib.h"
#include "ArduiPi_OLED.h"


namespace PiWars
{

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
      std::cerr << "Unable to find five way" << std::endl;
      exit(-1);
    }
    
    // Populate the menu
    _mainMenu->add(new MenuItem(menuItemInfo));
    _mainMenu->add(new MenuItem(menuItemBrains));
    _mainMenu->add(new MenuItem(menuItemShutdown));
    
    // Initialise the display
    _display->init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64);
    _display->begin();

    // Display the initial info
    updateDisplay();

  }
  
  PiWars::~PiWars() {
    // Display shutdown message
    _display->clearDisplay();
    _display->setTextSize(1);
    _display->setTextColor(WHITE);
    _display->setCursor(0,0);
    _display->print("Shutting down\n");
    _display->display();
    
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
    
    // Query the file descriptor so we can correctly wait for events
    fds[0].fd = _inputQueue->getFD();
    fds[0].events = POLLIN;
    fds[0].revents = 0;
      
    while(_running) {
      int result = poll(fds, 1, 1000);
      
      if(-1 == result) {
        std::cerr << "PiWars: Poll returned error" << std::endl;
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
  
          // Clear out any queued up input events
          while(!_inputQueue->empty()) {
            _inputQueue->pop(event);
            
            if(InputEventType::BUTTON == event.getType()) {
              std::cerr << "code " << event.getCode() << "value " << (int32_t)event.getButtonValue() << std::endl;
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
    if((int32_t)InputEventButtonValue::RELEASE == event.getButtonValue() )
    {
      // Is the menu not currently active?
      if(!_currentMenu) {
        // Is it the 'OK' button
        if(KEY_ENTER == event.getCode()) {
          // Display the main menu
          _currentMenu = _mainMenu;
        }
      }
      else {
        if(KEY_UP == event.getCode()) {
          _currentMenu->next();
        }
        else if(KEY_DOWN == event.getCode()) {
          _currentMenu->previous();
        }
        else if(KEY_ENTER) {
          bool isMainMenu = (_currentMenu == _mainMenu);
          std::string currentEntry = _currentMenu->current();
          
          std::cerr << "Processing " << _currentMenu->current() << std::endl;

          // Assume the menu will no longer be active after this
          _currentMenu = nullptr;
           
          // Are we on the main menu?
          if(isMainMenu) { 
            if(0 == currentEntry.compare(menuItemInfo)) {            
            }
            else if(0 == currentEntry.compare(menuItemBrains)) {
              _currentMenu = _brains->menu();
            }
            else if(0 == currentEntry.compare(menuItemShutdown)) {
              _running = false;
            }
          }
          else {
            // Currently we assume it must be the 'Brains' menu
            _brains->selectMenuEntry(currentEntry);
          }
        }        
      }
      
      updateDisplay();
    }
  }
  
  void PiWars::updateDisplay() {
    // Display latest information on the display
    _display->clearDisplay();

    // Display some text
    _display->setTextSize(1);
    _display->setTextColor(WHITE);
    _display->setCursor(0,0);
    _display->print("Stats\n");
    _display->setCursor(0,10);
    _display->print("Current: ThreePointTurn\n");
    
    if(_currentMenu) {
      _currentMenu->render(_display, 40, 128);
    }
    
    _display->display();
    
  }

}
