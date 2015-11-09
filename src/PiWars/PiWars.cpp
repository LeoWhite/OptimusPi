#include "PiWars.h"
#include "Brains.h"
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

  PiWars::PiWars() 
    : _brains(new Brains())
    , _powertrain(new Powertrain())
    , _display(new ArduiPi_OLED())
    , _fiveWay(nullptr)
    , _inputQueue(nullptr)
    , _menuActive(false)
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
    
    // Initialise the display
    _display->init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64);
    _display->begin();

    // Display the initial info
    updateDisplay();

  }
  
  PiWars::~PiWars() {
    delete _fiveWay;
    delete _inputQueue;
    delete _brains;
    delete _powertrain;
    delete _display;
  }

  void PiWars::run() {
    struct pollfd fds[2];
  
    // Query the file descriptor so we can correctly wait for events
    fds[0].fd = _inputQueue->getFD();
    fds[0].events = POLLIN;
    fds[0].revents = 0;
      
    while(int result = poll(fds, 1, -1)) {
      if(-1 == result) {
        std::cerr << "PiWars: Poll returned error" << std::endl;
        continue;
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
      // Display the menu

      // Wait for the a selection

      // Check results

      // Select ThoughtProcess to run

      // or exit? 

      // Display 'currently running'?

      // On key press display 'Stop'

    }
  }
  
  void PiWars::processButton(const InputEvent &event) {
    // Is the menu not currently active?
    if(!_menuActive) {
      // Is it the 'OK' button
      if(KEY_ENTER == event.getCode() && (int32_t)InputEventButtonValue::RELEASE == event.getButtonValue()) {
        std::cerr << "OK button pressed, creating menu" << std::endl;
        generateMenu();
        _menuActive = true;
      }
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
    _display->setCursor(0,40);
    
    if(_menuActive) {
      _display->print("Select:\n");
      _display->setCursor(0,50);
      _display->print("| LineFollower      >\n");
      
      
      for(uint32_t i = 0; i < 5; i++) {
        uint32_t numItems = 5;
        uint32_t length = 128 / numItems;
        
        uint32_t startX = i * (length + 1);
  
        if(i == 3) {      
          _display->drawLine(startX, 62, startX + length, 62, WHITE);
        }
        else {
          _display->drawLine(startX, 62, startX + 1, 62, WHITE);
        }
      }
    }
    _display->display();
    
  }

}
