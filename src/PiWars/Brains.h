/**
 * The Brains of the robot keeps track of what activities its able
 * to do, and what activity is currently in progress.
 *
 * In future it may get extended to allow multiple activies (e.g. performing 
 * collision detection when other activities are enabled )
 */
 
#ifndef _PIWARS_BRAINS_H
#define _PIWARS_BRAINS_H

#include <atomic>
#include <algorithm>
#include <memory>
#include <thread>
#include <vector>

#include "ThoughtProcess.h"

namespace PiWars
{
  // Forward declarations
  class Menu;
  
  class Brains
  {
    public:
      Brains(); 
      ~Brains();
      
      // Adds a ThoughtProcess that can then be selected and
      // controlled by the Brain
      // 
      // @param thoughtProcess The process to add, the Brain will take control of it
      void addThoughtProcess(ThoughtProcess::ptr thoughtProcess);
      
      // Enables the specified through process
      // @param The thought process to enable
      //
      // @returns true if the process was enabled
      //          false otherwise (Process isn't available)
      bool enableThoughtProcess(ThoughtProcess::ptr process);
      
      // Enables the specified through process
      // @param Index of the thought process to enable
      //
      // @returns true if the process was enabled
      //          false otherwise (Process isn't available)
      bool enableThoughtProcess(std::size_t process);
      
      // Returns a menu containing all the active through processes
      // @returns A populated menu
      Menu *menu();

      // Selects the specific menu entry name, triggering
      // any related actions associated to it.
      //
      // @param The name of the menu entry that has been selected
      void selectMenuEntry(const std::string &entry);

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
      void stopCurrentThoughtProcess();
      static void currentProcessRun(std::atomic<bool> &running, ThoughtProcess::ptr &process);
      ThoughtProcess::vector _processes;
      ThoughtProcess::ptr _currentProcess;      
      std::thread *_currentProcessThread;
      std::atomic<bool> _currentProcessRunning; //<! Used to indicate when the thread should exit

  };
}
#endif
