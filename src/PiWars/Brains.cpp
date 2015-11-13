/**
 * The Brains of the robot keeps track of what activities its able
 * to do, and what activity is currently in progress.
 *
 * In future it may get extended to allow multiple activies (e.g. performing 
 * collision detection when other activities are enabled)
 */

#include <limits>
#include "Brains.h"
#include "Menu.h"
#include "ThoughtProcess.h"

#include <iostream>

namespace PiWars
{

// Static declarations for the menu
static std::string menuItemStop = "Stop";

Brains::Brains() 
  : _currentProcess(nullptr)
  , _currentProcessThread(nullptr)
  , _currentProcessRunning(false)
{
}

Brains::~Brains() {
  // Stop the current proces (if any) 
  stopCurrentThoughtProcess();
}

void Brains::addThoughtProcess(ThoughtProcess::ptr thoughtProcess) {
  // Add it to the vector.
  // IMPROVE: Do we want to check for duplicates?
  _processes.push_back(thoughtProcess);
}

const std::string Brains::currentThoughtProcess() const {
  static const std::string empty = "";

  // Is there a process currently selected?
  if(_currentProcess) {
    return _currentProcess->name();
  }
  // If no, return the emptry string
  else {
    return empty;
  }  
}

Menu *Brains::menu() {
  // We create a new menu each time to ensure it
  // has most uptodate information
  Menu *menu = new Menu();
  
  // Do we have an 'active' process?
  if(_currentProcess) {
    // Add a 'Stop' option
    menu->add(new MenuItem(menuItemStop));
  }
  
  // Add all the currently available processes
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
    stopCurrentThoughtProcess();
  }
  // Run through the list of processes
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
    // Pass through to enable
    enabled = enableThoughtProcess(_processes[process]);
  }
  
  return enabled;
}

bool Brains::enableThoughtProcess(ThoughtProcess::ptr process) {
  bool enabled = false;
  
  // Is it a valid process? And is it available?
  if(process && process->available()) {
    // Do we have an existing process to tidy up?
    stopCurrentThoughtProcess();
    
    // Select this as the current process
    _currentProcess = process;
    
    // Prepare the process    
    if(_currentProcess->prepare()) {
      // and set it off running in a background thread, so it doesn't block this one
      _currentProcessRunning = true;
      _currentProcessThread = new std::thread(currentProcessRun, std::ref(_currentProcessRunning), std::ref(_currentProcess));
      enabled = true;
    }
    else {
      std::cerr << "Failed to prepare process" << std::endl;
      _currentProcess.reset();
    }
  }
  
  return enabled;
}

void Brains::stopCurrentThoughtProcess() {
  if(_currentProcess) {
    // Tell it to stop
    _currentProcessRunning = false;

    // Wait for the running thread to stop
    _currentProcessThread->join();
    _currentProcessThread = nullptr;
        
    // and then clear it
    _currentProcess.reset();
  }
}

void Brains::currentProcessRun(std::atomic<bool> &running, ThoughtProcess::ptr &process) {
  // Just run the through process, safely in this thread
  process->run(running);
}

}
