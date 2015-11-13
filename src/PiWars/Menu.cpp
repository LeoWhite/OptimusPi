/**
 * Defines a Menu that can be output on the Display for users
 * to interact with.
 */

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

MenuItem::MenuItem(const std::string &name) : _name(name) {
}

Menu::Menu() : _currentEntry(0)
{
}

void Menu::add(MenuItem *item) {
  _entries.push_back(item);
}

void Menu::render(ArduiPi_OLED *display, uint32_t yOffset, uint32_t width) {
  // Anything to display?
  if(_currentEntry < _entries.size()) {
    display->setCursor(0, yOffset);

    // Output the current MenuItem
    display->print("Select:\n");
    display->setCursor(0, yOffset + 10);
    display->print(_entries[_currentEntry]->name().c_str());

    // Draw an indicator so the user can tell how many MenuItems
    // there are in the Menu, and where teh current one is
    uint32_t itemLength = width / _entries.size();
    for(uint32_t i = 0; i < _entries.size(); i++) {
      uint32_t startX = i * (itemLength + 1);

      // The current entry is represented by a solid line
      if(i == _currentEntry) {
        display->drawLine(startX, yOffset + 22, startX + itemLength, yOffset + 22, WHITE);
      }
      // All other entries are just a 'divider'
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

  // Is there anything in the menu?
  if(_entries.size()) {
    return _entries[_currentEntry]->name();
  }
  // If no menu, return the emptry string
  else {
    return empty;
  }
}

}
