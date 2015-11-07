/**
 * The Brains of the robot keeps track of what activities its able
 * to do, and what activity is currently in progress.
 *
 * In future it may get extended to allow multiple activies (e.g. performing 
 * collision detection when other activities are enabled )
 */
 
#ifndef _PIWARS_BRAINS_H
#define _PIWARS_BRAINS_H

#include <algorithm>
#include <memory>
#include <vector>

#include "ThoughtProcess.h"

namespace PiWars
{
  class Brains
  {
    public:
      Brains(); 
      
      // Adds a ThoughtProcess that can then be selected and
      // controlled by the Brain
      // 
      // @param thoughtProcess The process to add, the Brain will take control of it
      void addThoughtProcess(ThoughtProcess::ptr thoughtProcess);
      
      // Enables the specified through process
      // @param Index of the thought process to enable
      //
      // @returns true if the process was enabled
      //          false otherwise (Process isn't available)
      bool enableThoughtProcess(std::size_t process);
      
      // Returns the vector of Processess to allow
      // iteration through it
      //
      // @returns the vector of known processes
      template <class F>
      void forEachThoughtProcess(F f) const
      {
        std::for_each(begin(_processes), end(_processes), f);
      }
      const ThoughtProcess::vector &processes() {
        return _processes;
      }
      
    private:
      ThoughtProcess::vector _processes;
      ThoughtProcess::ptr _currentProcess;      
  };
}
#endif
