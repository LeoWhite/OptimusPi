/**
 * PiWars
 *
 * A library of functionality targetted towards performing the PiWars 2015 challenges.
 */

// Our PiWars namespace to make sure we don't clash with any other APIs/libraries we
// may end up using
namespace PiWars {
  
// Forwards declaratinos
class Brains;

class PiWars {
  public:
    PiWars();
    
    ~PiWars() {
    }

    /**
     * Returns the 'Brains' for this PiWars instance.
     */     
    Brains *getBrains() { return _brains; }
    
  private:
    
    Brains *_brains;
};

}