# The PiWars library
include_directories("/usr/include/libevdev-1.0/libevdev/" "/usr/local/include/")

add_library(PiWars SHARED PiWars.cpp Brains.cpp InputManager.cpp InputDevice.cpp InputEvent.cpp Powertrain.cpp I2C.cpp SensorRTIMU.cpp SensorVL6180.cpp SensorQTR8RC.cpp ThoughtProcess_Manual.cpp ThoughtProcess_Proximity.cpp ThoughtProcess_LineFollower.cpp ThoughtProcess_StraightLine.cpp ThoughtProcess_ThreePointTurn.cpp)

target_link_libraries(PiWars evdev pthread pigpiod_if RTIMULib ArduiPi_OLED)

