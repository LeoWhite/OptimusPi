#include <limits>
#include "Brains.h"
#include "ThoughtProcess.h"

#include <iostream>

namespace PiWars
{

Brains::Brains() {

}

void Brains::addThoughtProcess(ThoughtProcess::ptr  thoughtProcess) {
  // Add it to the vector.
  // IMPROVE: Do we want to check for duplicates?
  _processes.push_back(thoughtProcess);
}

bool Brains::enableThoughtProcess(std::size_t process) {
  bool enabled = false;
  
  // Is it a valid offset? And is it active?
  if(process < _processes.size() && _processes[process]->available()) {
    // Do we have an existing process to tidy up?
    if(_currentProcess) {
      // Tell it to stop
      _currentProcess->stop();
      
      // and then clear it
      _currentProcess.reset();
    }
    
    // Select the processes
    _currentProcess = _processes[process];
    
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
