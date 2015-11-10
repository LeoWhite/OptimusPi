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

  PiWars::PiWars() 
    : _running(false)
    , _brains(new Brains())
    , _powertrain(new Powertrain())
    , _display(new ArduiPi_OLED())
    , _fiveWay(nullptr)
    , _inputQueue(nullptr)
    , _menuActive(false)
    , _menu(new Menu())
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
    _menu->add(new MenuItem("Info"));
    _menu->add(new MenuItem("Brains"));
    _menu->add(new MenuItem("Stop"));
    _menu->add(new MenuItem("Shutdown"));
    
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
      int result = poll(fds, 1, -1);
      
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
    // We only respond to key release
    if((int32_t)InputEventButtonValue::RELEASE == event.getButtonValue() )
    {
      // Is the menu not currently active?
      if(!_menuActive) {
        // Is it the 'OK' button
        if(KEY_ENTER == event.getCode()) {
          _menu->update();
          _menuActive = true;
        }
      }
      else {
        if(KEY_UP == event.getCode()) {
          _menu->next();
        }
        else if(KEY_DOWN == event.getCode()) {
          _menu->previous();
        }
        else if(KEY_ENTER) {
          std::cerr << "Processing " << _menu->current() << std::endl;
           
          // Assume the menu will no longer be active
          _menuActive = false;
 
          if(0 == _menu->current().compare("Info")) {
          }
          else if(0 == _menu->current().compare("Brains")) {
          }
          else if(0 == _menu->current().compare("Stop")) {
          }
          else if(0 == _menu->current().compare("Shutdown")) {
            _running = false;
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
    
    if(_menuActive) {
      _menu->render(_display, 40, 128);
    }
    
    _display->display();
    
  }

}
