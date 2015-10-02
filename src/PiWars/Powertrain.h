/**
 * The Powertrain represents the Engine, gearbox, drive and wheels of the robot.
 * Altogether this allows the robot to be driven around and moved.
 */
#include <cstdint>

namespace PiWars {
  // Forward declaration
  class InputDevice;

  // An Engine represents one or more motors with the ability to turn
  // left or right, either via tank track driving, or with a steering column.
  class Powertrain {
    public:
      Powertrain();
      ~Powertrain();

      // Stop the robot
      void stop();

      // Explicitly set the power of the motors
      // @param left Power applied to the 'left wheel' from -1.0 to 1.0
      // @param right Power applied to the 'right wheel' from -1.0 to 1.0
      //
      // @returns true if power was set
      //          false if input range invalid, or request wasn't passed to the Engine
      bool setPower(float left, float right);

      // Get the current power levels
      void getPower(float &left, float &right);

      // Sets the maximum amount of power that should be applied
      // in the range 0.0 to 1.0. This value is used to modify any
      // 'setPower' requests allowing the overall power level
      // of the system be altered in a single place.
      // e.g. if the limiter is set to '0.5' then a power level
      // of '1.0' will be reduced to '0.5' and a power level
      // of '0.5' will be reduced to '0.25'
      //
      // @returns true if limiter was set
      //          false if limiter range is invalid
      bool setLimiter(float limiter);

      // Allows connecting an InputDevice to allow manual control
      // of the Powertrain
      //
      // @param device - The Input device to connect
      // @param leftAxis - The axis to use for controlling the left side (e.g. ABS_X)
      // @param rightAxis - The axis to use for controlling the right side (e.g. ABS_Z)
      //
      // @returns true if input device was connected
      //          false if unable to register the specified axes
      bool setInputDevice(InputDevice &device, uint32_t leftAxis, uint32_t rightAxis);

    private:
      float _powerLeft; //!< Amount of power to apply to the left motor
      float _powerRight; //!< Amount of power to apply to the right motor
      
      float _powerLimiter; //!< What to limit the power
      
      int _i2cHandle; //!< Handle to talk ot the motor via i2c
  };

}
