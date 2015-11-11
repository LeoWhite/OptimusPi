#include <limits>
#include "Brains.h"
#include "Menu.h"
#include "ThoughtProcess.h"

#include <iostream>

namespace PiWars
{

static std::string menuItemStop = "Stop";

Brains::Brains() {

}

void Brains::addThoughtProcess(ThoughtProcess::ptr  thoughtProcess) {
  // Add it to the vector.
  // IMPROVE: Do we want to check for duplicates?
  _processes.push_back(thoughtProcess);
}

Menu *Brains::menu() {
  Menu *menu = new Menu();
  
  // Do we have an 'active' process?
  if(_currentProcess) {
    // Add a 'Stop' option
    menu->add(new MenuItem(menuItemStop));
  }
  
  // List out all the available processes
  for(auto n : _processes) {
    if(n->available()) {
      menu->add(new MenuItem(n->name()));
    }
  }
  
  return menu;  
}

void Brains::selectMenuEntry(const std::string &entry) {
  // Is it the special 'stop' entry?
  if(0 == entry.compare(menuItemStop)) {
    if(_currentProcess) {
      // Tell it to stop
      _currentProcess->stop();
      
      // and then clear it
      _currentProcess.reset();
    }
  }
  // Check through the list of processes
  else {
    for(auto n : _processes) {
      // Does it match?
      if(0 == entry.compare(n->name())) {
        // Then attempt to enable it
        enableThoughtProcess(n);
        break;
      }
    }
  }
}

bool Brains::enableThoughtProcess(std::size_t process) {
  bool enabled = false;
  
  // Is it a valid offset? And is it active?
  if(process < _processes.size() && _processes[process]->available()) {
    enabled = enableThoughtProcess(_processes[process]);
  }
  
  return enabled;
}

bool Brains::enableThoughtProcess(ThoughtProcess::ptr process) {
  bool enabled = false;
  
  // Is it a valid process? And is it available?
  if(process && process->available()) {
    // Do we have an existing process to tidy up?
    if(_currentProcess) {
      // Tell it to stop
      _currentProcess->stop();
      
      // and then clear it
      _currentProcess.reset();
    }
    
    // Select the processes
    _currentProcess = process;
    
    std::cerr << "Enabling process " << _currentProcess->name() << std::endl;
      
    if(_currentProcess->prepare()) {
      _currentProcess->run();
      enabled = true;
    }
    else {
      std::cerr << "Failed to prepare process" << std::endl;
      _currentProcess.reset();
    }
  }
  
  return enabled;
}

}
