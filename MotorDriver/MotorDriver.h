
//------------------------- Define function of each I/O pin -------------------------------------------------

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1

#define CURRENT_OVERLOAD_CUTOUT 10000


#define pwm1pin   5
#define ina1pin   7
#define inb1pin   8
#define cs1pin    2  // Current sense

#define pwm2pin   6
#define ina2pin   4
#define inb2pin   9
#define cs2pin    3  // Current sense

// Define a structure to hold information about the motors
typedef struct Motor {
  int power; // Current power level of the motor from -100 to 100
  bool brake; // If true then enable the electric brake
  int current;   // Current being pulled by the motor in milli-amps
  volatile int encoderCount; // How far this motor has turned since the count was last reset
};


