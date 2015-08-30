#!/usr/bin/env python
# Requires evdev (sudo pip-3.2 install evdev)
from sense_hat import SenseHat
import evdev

# Read in from the physically attached keyboard 
# (For some reason not getting input from the Sense HAT joystick)
dev = evdev.InputDevice('/dev/input/event0')
 
# Initialise the sense HAT, also ready for use
sense = SenseHat()
sense.clear()

# reads in the temperature and displays it on the LED matrix
def display_temp():
  value = sense.get_temperature()
  message = str.format("%.1fC" % value)
  sense.show_message(message, text_colour=(255,0,0))

# reads in the humidity and displays it on the LED matrix
def display_humidity():
  value = sense.get_humidity()
  message = str.format("%.1f%%rh" % value)
  sense.show_message(message, text_colour=(0,255,0))
  
# reads in the pressure and displays it on the LED matrix
def display_pressure():
  value = sense.get_pressure()
  message = str.format("%.1f mbar" % value)
  sense.show_message(message, text_colour=(0,0,255))

try:
  while True:
      for event in dev.read_loop():
        if event.type == evdev.ecodes.EV_KEY:
           data = evdev.categorize(event)
           
           # Is it a keydown? We don't want to trigger twice (On the down and up)
           if data.keystate == 1:
             if data.scancode == evdev.ecodes.KEY_DOWN:
                display_temp()
             elif data.scancode == evdev.ecodes.KEY_UP:
                display_humidity()
             elif data.scancode == evdev.ecodes.KEY_LEFT:
               display_pressure()
        
except KeyboardInterrupt:
    # Clear the display
    sense.clear()
