# The PiWars library
include_directories("/usr/include/libevdev-1.0/libevdev/")

add_library(PiWars SHARED PiWars.cpp Brains.cpp InputManager.cpp InputDevice.cpp)

target_link_libraries(PiWars evdev pthread)

