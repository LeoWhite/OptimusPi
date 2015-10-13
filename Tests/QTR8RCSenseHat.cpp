/*****************************************************************************
 * File: QTR8RCSenseHat.cpp
 * Author: Leo White
 * Available from: https://github.com/LeoWhite/OptimusPi/blob/master/Tests/QTRSenseHat.cpp
 *
 * Some example code that uses the Pololu QTR-8RC Reflectance Sensor Array
 * (https://www.pololu.com/product/961) connected to a Raspberry Pi
 * via the GPIO pins to control the LED matrix on a Sense HAT.
 *
 * This code sets up the sensor using the pigpio library (http://abyz.co.uk/rpi/pigpio/)
 * and then reads the sensor array every second, outputting the state of
 * each sensor as a 0 (white) or 1 (black) value.
 *
 * SenseHAT code taken from the SenseHAT example installed at
 * /usr/src/sense-hat/examples/snake/snake.c
 *
 * Build using g++ -o QTR8RCSenseHat -lpthread -lpigpio  QTR8RCSenseHat.cpp
 * Run using sudo ./QTR8RCSenseHat
 */

#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <sstream>
#include <string.h>
#include <sys/mman.h>
#include <dirent.h>
#include <errno.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include "pigpio.h"


// The cut off level for declaring if the sensor is above white or black
// Note: This value will vary depending on distance from the sensor to the
// surface, any stray IR light output, actual shade of the white/black.
#define WHITE_LEVEL 256

// The GPIO pin of the LED_ON connection
#define LED_ON 16

// The GPIO pins that the eight IR sensors are connected to
uint32_t sensorGPIOs[] = { 17, 18, 19, 20, 21, 22, 5, 6 };

// Details of the pigpio notification pipe that we'll be
// reading the GPIO changes from.
uint32_t notificationGPIOs = 0;
int notifyPipe = -1;
int notifyPipeFD = -1;

// The SenseHAT's LED Matrix
struct fb_t {
	uint16_t pixel[8][8];
};

// Pointer to the FB
struct fb_t *fb;

#define DEV_FB "/dev"
#define FB_DEV_NAME "fb"

static int is_framebuffer_device(const struct dirent *dir)
{
	return strncmp(FB_DEV_NAME, dir->d_name,
		       strlen(FB_DEV_NAME)-1) == 0;
}

static int open_fbdev(const char *dev_name)
{
	struct dirent **namelist;
	int i, ndev;
	int fd = -1;
	struct fb_fix_screeninfo fix_info;

	ndev = scandir(DEV_FB, &namelist, is_framebuffer_device, versionsort);
	if (ndev <= 0)
		return ndev;

	for (i = 0; i < ndev; i++)
	{
		char fname[64];
		char name[256];

		snprintf(fname, sizeof(fname),
			 "%s/%s", DEV_FB, namelist[i]->d_name);
		fd = open(fname, O_RDWR);
		if (fd < 0)
			continue;
		ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
		if (strcmp(dev_name, fix_info.id) == 0)
			break;
		close(fd);
		fd = -1;
	}
	for (i = 0; i < ndev; i++)
		free(namelist[i]);

	return fd;
}

/**
 * Initialises the pigpio library and sensor array ready for use
 */
void setup(void) {
	int fbfd = 0;

  // Initialise the pigpio library
  gpioInitialise();

  // Configure the LED_ON pin as an output, and set to 'low'
  gpioSetMode(LED_ON, PI_OUTPUT);
  gpioWrite(LED_ON, 0);

  // We need to monitor changes on multiple GPIO pins and the time their value
  // change. So we're going to use the 'notify' pipe to do this
  notifyPipe = gpioNotifyOpen();

  if(notifyPipe < 0) {
    std::cerr << "Failed to open notification pipe! Exitting." << std::endl;
    exit(-1);
  }

  // Generate the path to the notify pipe and open it
  std::ostringstream notifyPathStream;
  notifyPathStream << "/dev/pigpio" << notifyPipe;

  notifyPipeFD = open(notifyPathStream.str().c_str(), O_RDONLY|O_NONBLOCK);
  if(notifyPipeFD < 0) {
    std::cerr << "Failed to open notify path [" << notifyPathStream.str() << "]" << std::endl;
    exit(-1);
  }

  // We want to monitor all 8 GPIOs
  notificationGPIOs = 0;
  for(size_t i = 0; i < 8; i++) {
    notificationGPIOs |= (1 << sensorGPIOs[i]);
  }

  // Disable any pull up resistors
  // Is this needed?
  for(size_t i = 0; i < 8; i++) {
    //gpioSetPullUpDown(sensorGPIOs[i], PI_PUD_DOWN);
  }
  
  // Connect to the SenseHAT FB
	fbfd = open_fbdev("RPi-Sense FB");
	if (fbfd <= 0) {
		std::cerr << "Error: cannot open framebuffer device." << std::endl;
		exit(-1);
	}
	
	fb = (fb_t *)mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (!fb) {
		std::cerr << "Failed to mmap framebuffer!" << std::endl;
		exit(-1);
	}
	memset(fb, 0, 128);
  
}

/**
 * Reads in a single set of readings from the sensor
 *
 * @param sensorDiff Filled in with the 'decay' time of each sensor
 */
void readSensor(uint32_t (&sensorDiff)[8]) {
  uint32_t startTime;

  // Turn on the IR LEDs
  gpioWrite(LED_ON, 1);

  // Configure all the sensors for output
  for(size_t i = 0; i < 8; i++) {
    gpioSetMode(sensorGPIOs[i], PI_OUTPUT);
    gpioWrite(sensorGPIOs[i], 1);
  }

  // Wait for the sensors to become 'charged'
  usleep(20);

  // Start listening for notifications
  if(gpioNotifyBegin(notifyPipe, notificationGPIOs) < 0) {
    std::cerr << "Failed to begin notification" << std::endl;
    exit(-1);
  }

  // Remember our 'start' time
  startTime = gpioTick();

  // And switch all the GPIOs to input
  for(size_t i = 0; i < 8; i++) {
    gpioSetMode(sensorGPIOs[i], PI_INPUT);
  }

  // wait for the sensors to 'discharge'
  usleep(1500);

  // Pause notifications
  gpioNotifyPause(notifyPipe);

  // turn off the IR LEDs
  gpioWrite(LED_ON, 0);

  // Process results
  gpioReport_t report;
  bool dataRead[8] = {0};

  // Loop through all the results queued up in the pipe
  while(sizeof(report) == read(notifyPipeFD, &report, sizeof(report))) {
    // Not interested in any of the flags (e.g. keep alive) so we
    // skip them
    if(report.flags) {
      continue;
    }

    // We need to detect when a GPIO has from high to low,
    // so for each message walk through all the sensors to
    // see if they have changed
    for(size_t i = 0; i < 8; i++) {
      // Has the sensor become 'low' for the first time
      if(!dataRead[i] && 0 == ((1 << sensorGPIOs[i]) & report.level)) {
        // Work out how long it took, and indicate we've gotten
        // a result for this sensor
        sensorDiff[i] = report.tick - startTime;
        dataRead[i] = true;
      }
      // We may have gotten a spurious 'low', so if the GPIO goes high again then
      // reset the 'dataRead' for this sensor
      else if(dataRead[i] && ((1 << sensorGPIOs[i]) & report.level)) {
        dataRead[i] = false;
      }
    }
  }
}

/**
 * Takes a set of readings from the sensor and updates the framebuffer
 *
 * @param sensorDiff The sensor reading
 */
void updateFB(uint32_t (&sensorDiff)[8]) {
  // Shuffle the current framebuffer across one line
  for(size_t i = 0; i < 8; i++) {
    memmove(&fb->pixel[i][1], &fb->pixel[i][0], 7 * sizeof(uint16_t));
  }
  
  // Insert the new data
  for(size_t i = 0; i < 8; i++) {
    if(sensorDiff[i] && sensorDiff[i] <= WHITE_LEVEL) {
      fb->pixel[i][0] = 0xFFFF;
    }
    else {
      fb->pixel[i][0] = 0;
    }
  }  
}

int main(void)
{
  int value = 1;

  // Setup the sensor ready for captures
  setup();

  // Loop forever
  while(1) {
    uint32_t sensorDiff[8] = {0};

    // Read in the current sensor reading
    readSensor(sensorDiff);

    std::cout << "Sensor values ";

    // Go through each sensor, outputting its level
    for(size_t i = 0; i < 8; i++) {
      if(sensorDiff[i] && sensorDiff[i] <= WHITE_LEVEL) {
        std::cout << "0";
      }
      else {
        std::cout << "1";
      }      

      if(i < 7) {
        std::cout << ":";
      }

    }

    std::cout << std::endl;

    updateFB(sensorDiff);
    
    // Sleep for a second before taking the next reading
    usleep(500000);
  }

  // We currently never get here, but if in future we
  // do then tidy up
  gpioNotifyClose(notifyPipe);

  gpioTerminate();

  return EXIT_SUCCESS;
}
