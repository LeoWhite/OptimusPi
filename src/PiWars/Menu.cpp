#include <limits>
#include "Menu.h"
#include "ThoughtProcess.h"
#include <string>

#include <iostream>

// Include last as it defines 'max' and 'min' macros
#include "ArduiPi_OLED_lib.h"
#include "ArduiPi_OLED.h"

namespace PiWars
{

  MenuItem::MenuItem(const std::string &name) : _name(name)
  {
  }
      


  MenuItemSubMenu::MenuItemSubMenu(const std::string &name,  Menu *menu, const Menu *parent)
    : MenuItem(name)
    , _menu(menu)
    , _parent(parent)
  {
  }
    
  MenuItemValue::MenuItemValue(const std::string &name,  const int32_t value)
    : MenuItem(name)
    , _value(value)
  {
  }
      

  Menu::Menu() : _currentEntry(0)
  {
  }

  void Menu::add(MenuItem *item) {
    _entries.push_back(item);
  }
  
  void Menu::update() {
  }

  void Menu::render(ArduiPi_OLED *display, uint32_t yOffset, uint32_t width) {
    // Anything to display?
    if(_currentEntry < _entries.size()) {
      display->setCursor(0, yOffset);
  
      display->print("Select:\n");
      display->setCursor(0, yOffset + 10);
      display->print(_entries[_currentEntry]->name().c_str());
        
      for(uint32_t i = 0; i < _entries.size(); i++) {
        uint32_t length = width / _entries.size();
        
        uint32_t startX = i * (length + 1);
  
        if(i == _currentEntry) {      
          display->drawLine(startX, yOffset + 22, startX + length, yOffset + 22, WHITE);
        }
        else {
          display->drawLine(startX, yOffset + 22, startX + 1,yOffset + 22, WHITE);
        }
      }
    }
 }  
 
  void Menu::next() {
    if((_currentEntry + 1) < _entries.size()) {
      _currentEntry++;
    }
  }

  void Menu::previous() {
    if(_currentEntry) {
      _currentEntry--;
    }
  }

  const std::string &Menu::current() {
    static const std::string empty = "";
      
    if(_entries.size()) {
      return _entries[_currentEntry]->name();
    }
    else {
      return empty;
    }
  }

}
