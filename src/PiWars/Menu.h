/**
 * Defines a Menu that can be output on the Display for users
 * to interact with.
 */
 
#ifndef _PIWARS_MENU_H
#define _PIWARS_MENU_H

#include <algorithm>
#include <memory>
#include <vector>

class ArduiPi_OLED;
namespace PiWars
{
  class Menu;
  
  class MenuItem {
    public:
      MenuItem(const std::string &name);
      
      const std::string &name() { return _name; }
      
    protected:
      std::string _name;        
  };

  class MenuItemSubMenu : public MenuItem {
    public:
      MenuItemSubMenu(const std::string &name,  Menu *menu, const Menu *parent);
      
    private:
      const Menu *_parent;
      Menu *_menu;
  };

  class MenuItemValue : public MenuItem {
    public:
      MenuItemValue(const std::string &name,  const int32_t value);
      
      int32_t value() { return _value; }
      
    private:
      int32_t _value;
  };
  
  class Menu
  {
    public:
      Menu(); 

      // Add an entry
      void add(MenuItem *item);
      
      // Update the menu to reflect any 'dynamic' information
      void update();

      // Render the menu to the display at the specified offset.
      // We will always draw two lines of information.
      void render(ArduiPi_OLED *display, uint32_t yOffset, uint32_t width);

      // Makes the next entry in the menu the active one
      void next();

      // Makes the previous entry in the menu the active one
      void previous();

      // Returns the name of the current menu entry
      const std::string &current();
        
    private:
      std::vector<MenuItem *> _entries;
      uint32_t                _currentEntry;
  };
}
#endif
