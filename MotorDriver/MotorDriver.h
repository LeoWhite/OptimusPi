
//------------------------- Define function of each I/O pin -------------------------------------------------

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1

#define CURRENT_OVERLOAD_CUTOUT 20000


#define pwm1pin   5 
#define ina1pin   7
#define inb1pin   8
#define cs1pin    2  // Current sense

#define pwm2pin   6
#define ina2pin   4
#define inb2pin   9
#define cs2pin    3  // Current sense
  

typedef struct Motor {
  int power;
  bool brake;
  int current;
  volatile int encoderCount;
};

