/**
 * PiWars
 *
 * A library of functionality targetted towards performing the PiWars 2015 challenges.
 */
#ifndef _PIWARS_PIWARS_H
#define _PIWARS_PIWARS_H

// Our PiWars namespace to make sure we don't clash with any other APIs/libraries we
// may end up using
namespace PiWars {

// Forwards declaratinos
class Brains;
class Powertrain;

class PiWars {
  public:
    PiWars();

    ~PiWars();

    /**
     * Returns the 'Brains' for this PiWars instance.
     */
    Brains *brains() { return _brains; }

    /**
     * Returns the 'PowerTrain' for this PiWars instance.
     */
    Powertrain *powertrain() { return _powertrain; }

  private:

    Brains *_brains;
    Powertrain *_powertrain;
};

}
#endif
