#!/usr/bin/env python
 
import pygame
import time
import os, sys, math
from collections import namedtuple
import struct

import pigpio

# Key mappings
PS3_SELECT = 0
PS3_L3 = 1
PS3_R3 = 2
PS3_START = 3
PS3_DPAD_UP = 4
PS3_DPAD_RIGHT = 5
PS3_DPAD_DOWN = 6
PS3_DPAD_LEFT = 7
PS3_L2 = 8
PS3_R2 = 9
PS3_L1 = 10
PS3_R1 = 11
PS3_TRIANGLE = 12
PS3_CIRCLE = 13
PS3_CROSS =  14
PS3_SQUARE = 15
PS3_PLAYSTATION = 16

PS3_AXIS_LEFT_H = 0
PS3_AXIS_LEFT_V = 1
PS3_AXIS_RIGHT_H = 2
PS3_AXIS_RIGHT_V = 3
PS3_AXIS_DPAD_UP = 8
PS3_AXIS_DPAD_RIGHT = 9
PS3_AXIS_DPAD_DOWN = 10
PS3_AXIS_DPAD_LEFT = 11 
PS3_AXIS_L2 = 12
PS3_AXIS_R2 = 3
PS3_AXIS_L1 = 14
PS3_AXIS_R1 = 15
PS3_AXIS_TRIANGLE = 16
PS3_AXIS_CIRCLE = 17
PS3_AXIS_CROSS =  18
PS3_AXIS_SQUARE = 19
PS3_AXIS_PLAYSTATION = 16

# The I2C address of the arduino
I2CAddress = 0x07

# The default power limit for the motors as a percentage. 
# This helps keep the chassis controllable by limiting its max speed
POWER_LIMITER_DEFAULT = 75

# Initialise the pygame library, ready for use
pygame.init()

# sends the i2c command to the Arduino, automatically adding a checksum
#
# cmd - the byte command to send
# message - any extra arguments to send
def sendMessage(cmd, message):
  newMessage =  cmd + message
  
  # Calculate checksum
  CS = len(newMessage)
  for i in range(0, len(newMessage)):
    CS = (CS & 0xFF ^ ord(newMessage[i:i+1]) & 0xFF)
    
  finalMessage = newMessage + chr(CS)
  
  # Send the message  
  pi.i2c_write_device(h, finalMessage)
  
# reads a message back from the Arduino, vaidating
# the checksum
#
# length - The expected length of the message  
def readMessage(length):
    # Read in data plus checksum
    (count, status) = pi.i2c_read_device(h, length + 1)
    
    if count == (length + 1):
      CS = length;
      for i in range(0, length):
        CS = (CS & 0xFF ^ ord(status[i:i+1]) & 0xFF)
      
      if CS == ord(status[length:length+1]):
        # Process checksum,
        return status[:length]
      else:
        print "Checksum mismatch!"
        
    return 0

  
# Stops the robot
def stop():
  # We use a try/catch block in case there is an error reading or writing
  # to the i2c file handle
  try:
    # Send the 'stop' request
    sendMessage("\x11", "")
    time.sleep(0.001)
    result = readMessage(1);
    if ord(result[0:1]) != 0x11:
      print "Failed to stop!"
  except:
    print "Failed to stop motors!"


    
# Set the motor power to the specifiedvalues
#
# leftMotor - Power for left motor. -100 to 100
# rightMotor - Power for right motor. -100 to 100
def setMotors(leftMotor, rightMotor):
  # We use a try/catch block in case there is an error reading or writing
  # to the i2c file handle
  try:
    # We are sending two arguments, so we use the struct.pack code
    # to convert them into a byte stream
    sendMessage("\x12", struct.pack("!hh", leftMotor, rightMotor))
    time.sleep(0.001)
    result = readMessage(1);
    if ord(result[0:1]) != 0x12:
      print "Failed to set motors!"
  except:
    print "Failed to set motors!"
    

# Wait for a joystick to become available before running the
# rest of the script
while pygame.joystick.get_count() == 0:
  print 'waiting for joystick count = %i' % pygame.joystick.get_count()
  pygame.joystick.quit()
  time.sleep(1)
  pygame.joystick.init()


# Get a handle on the joystick
j = pygame.joystick.Joystick(0)
j.init()

print 'Initialized Joystick : %s' % j.get_name()

LeftTrack = 0
RightTrack = 0
PowerLimiter = POWER_LIMITER_DEFAULT

# Configure pigpio
pi = pigpio.pi()
h = pi.i2c_open(1, I2CAddress)

try:
    # Only allow axis and button events
    pygame.event.set_allowed([pygame.JOYAXISMOTION, pygame.JOYBUTTONDOWN])

    # Make sure the motors are stopped
    stop()
    
    while True:
        events = pygame.event.get()
        for event in events:
          UpdateMotors = 0
          NewLeftTrack = 0;
          NewRightTrack = 0;
          
          # Check if the analogue sticks have changed position
          # and update the motors accordingly
          if event.type == pygame.JOYAXISMOTION:
            if event.axis == PS3_AXIS_LEFT_V:
              NewLeftTrack = -(math.ceil(event.value * PowerLimiter))
              if NewLeftTrack != LeftTrack:
                LeftTrack = NewLeftTrack
                UpdateMotors = 1
            elif event.axis == PS3_AXIS_RIGHT_V:
              NewRightTrack = -(math.ceil(event.value * PowerLimiter))
              if NewRightTrack != RightTrack:
                RightTrack = NewRightTrack
                UpdateMotors = 1

          if event.type == pygame.JOYBUTTONDOWN:
            # Change the power limit to its default
            if event.button == PS3_DPAD_LEFT:
              PowerLimiter = POWER_LIMITER_DEFAULT;
            # Change the power limit to its max
            elif event.button == PS3_DPAD_RIGHT:
              PowerLimiter = 100;
            # Increase the power limit
            elif event.button == PS3_DPAD_UP:
              PowerLimiter += 5;
              if PowerLimiter > 100:
                PowerLimiter = 100;
            # Increase the power limit
            elif event.button == PS3_DPAD_DOWN:
              PowerLimiter -= 5;
              if PowerLimiter < POWER_LIMITER_DEFAULT:
                PowerLimiter = POWER_LIMITER_DEFAULT;
                
          if UpdateMotors:
            #print 'LeftTrack %f' % LeftTrack
            #print 'RightTrack %f' % RightTrack              
            setMotors(LeftTrack, RightTrack)  
              
        
        
except KeyboardInterrupt:
    # Turn off the motors
    stop()
    pi.stop()
    j.quit()
