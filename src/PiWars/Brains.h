/**
 * The Brains of the robot keeps track of what activities its able
 * to do, and what activity is currently in progress.
 *
 * In future it may get extended to allow multiple activies (e.g. performing 
 * collision detection when other activities are enabled )
 */
 
#ifndef _PIWARS_BRAINS_H
#define _PIWARS_BRAINS_H

#include <memory>
#include <vector>

namespace PiWars
{
  class ThoughtProcess;
  
  class Brains
  {
    public:
      Brains(); 
      
      // Adds a ThoughtProcess that can then be selected and
      // controlled by the Brain
      // 
      // @param thoughtProcess The process to add, the Brain will take control of it
      void addThoughtProcess(std::shared_ptr<ThoughtProcess>thoughtProcess);
      
      // Enables the specified through process
      // @param Index of the thought process to enable
      bool enableThoughtProcess(std::size_t process);
      
      
    private:
      std::vector<std::shared_ptr<ThoughtProcess>> _processes;
      std::shared_ptr<ThoughtProcess> _currentProcess;      
  };
}
#endif
