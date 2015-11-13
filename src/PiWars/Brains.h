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
      // @param thoughtProcess The process to add
      void addThoughtProcess(ThoughtProcess::ptr thoughtProcess);
      
      // Enables the specified ThoughtProcess
      //
      // @param process The thought process to enable
      // @returns true if the process was enabled
      //          false otherwise (e.g. Process isn't available)
      bool enableThoughtProcess(ThoughtProcess::ptr process);
      
      // Enables the specified ThoughtProcess
      //
      // @param process Index of the thought process to enable
      //
      // @returns true if the process was enabled
      //          false otherwise (Process isn't available)
      bool enableThoughtProcess(std::size_t process);
      
      // Queries the name of the current process (if any)
      //
      // @returns The name of the currently running process
      const std::string currentThoughtProcess() const;

      // Returns a menu containing all the currently avaiable
      // ThoughtProcesses
      //
      // @returns A populated menu
      Menu *menu();

      // Selects the specific menu entry name, triggering
      // any related actions associated to it.
      //
      // @param entry The name of the menu entry that has been selected
      void selectMenuEntry(const std::string &entry);

      // Returns the vector of Processess to allow
      // iteration through them
      //
      // @returns the vector of known processes
      const ThoughtProcess::vector &processes() {
        return _processes;
      }
      
    private:
      // Tell the current ThoughtProcess (if any) to stop, and
      // waits for it to finish
      void stopCurrentThoughtProcess();
      
      // The thread function that the current process will run inside to
      // avoid it blocking the main thread, and to avoid the ThreadProcess's
      // having to implement their own threads.
      static void currentProcessRun(std::atomic<bool> &running, ThoughtProcess::ptr &process);
        
      ThoughtProcess::vector _processes; //<! All the selectable ThoughtProcesses
      ThoughtProcess::ptr _currentProcess; //<! The currently running ThoughtProcess (if any)
      std::thread *_currentProcessThread; //<! The thread for the current process
      std::atomic<bool> _currentProcessRunning; //<! Used to indicate when the currentProcess should exit
  };
}
#endif
