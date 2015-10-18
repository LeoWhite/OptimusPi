#ifndef _PIWARS_BRAINS_H
#define _PIWARS_BRAINS_H

namespace PiWars
{
  class ThoughtProcess;
  
  class Brains
  {
    public:
      Brains();
      
    private:
      ThoughtProcess *_currentProcess;
  };
}
#endif
