/**
 * Defines a Menu that can be output on the Display for users
 * to interact with.
 */

#ifndef _PIWARS_MENU_H
#define _PIWARS_MENU_H

#include <algorithm>
#include <memory>
#include <vector>

// Fordwared declare classes
class ArduiPi_OLED;

namespace PiWars
{
  class Menu;

  // Represents a single menu entry in the menu
  class MenuItem {
    public:
      // Creates a MenuItem with the specified name
      //
      // @param name The name of this MenuItem that will be displayed in the menu
      MenuItem(const std::string &name);

      // Returns the name of this MenuItem
      //
      // @returns The name
      const std::string &name() { return _name; }

    protected:
      std::string _name; //<! The stored name of this menu entry
  };

  // Represents a Menu, which is a list of 0 or more MenuItems
  class Menu
  {
    public:
      Menu();

      // Add an entry to the menu
      //
      // @param item The MenuItem to add
      void add(MenuItem *item);

      // Render the menu to the display at the specified offset.
      // The menu will always draw two lines of information.
      //
      // @param display The display to render the menu on
      // @param yOffset The yOffset to start rendering at
      // @param width   The width of the display
      void render(ArduiPi_OLED *display, uint32_t yOffset, uint32_t width);

      // Makes the next entry in the menu the active one
      void next();

      // Makes the previous entry in the menu the active one
      void previous();

      // Gets the name of the current menu entry
      //
      // @returns the name of the current MenuItem
      //          or an empty string if no MenuItems
      const std::string &current();

    private:
      std::vector<MenuItem *> _entries; //<! Contains all the MenuItems in this menu
      uint32_t _currentEntry; //<! Indicates the currently selected entry
  };
}
#endif
