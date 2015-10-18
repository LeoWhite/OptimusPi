#include "I2C.h"
#include <mutex>

// The pigpiod_if header file doesn't protect itself when included from
// C++, so force it to be treated as 'C' here.
extern "C" {
#include "pigpiod_if.h"
}

#include <iostream>

namespace PiWars
{

std::once_flag initPIGPIODFlag;
  
I2C::I2C(uint8_t i2cAddress) : _i2cAddress(i2cAddress), _i2cHandle(-1) {
  // We need to initialise pigpiod once, regardless of how many 
  // i2c connections we establish
  std::call_once(initPIGPIODFlag, initPIGPIOD);
  
  // Establish a handle to talk to the i2c device
  _i2cHandle = i2c_open(1, _i2cAddress, 0);

}

I2C::~I2C() {
}

bool I2C::exists() {
  bool exists = false;
  
  if(0 == i2c_write_quick(_i2cHandle, 0)) {
    exists = true;
  }
  
  return exists;
}

bool I2C::writeBytes(const char *bytes, size_t length) {
  bool result = true;
  
  if(i2c_write_device(_i2cHandle, (char *)bytes, length) < 0 ) {
    std::cerr << "Failed to send message!" << std::endl;
    result = false;
  }  
  
  return result;
}

size_t I2C::readBytes(char *buffer, size_t length) {
  int read;
  
  read = i2c_read_device(_i2cHandle, buffer, length);
  
  // Did we fail?
  if(read < 0) {
    // Set to 0 for now
    read = 0;
  }
  
  return read;
}

bool I2C::writeByte(const uint8_t byte) {  
  if(i2c_write_byte(_i2cHandle, byte) >= 0) {
    return true;
  }
  
  return false;
}

bool I2C::readByte(uint8_t &byte) {
  int read;
  
  read = i2c_read_byte(_i2cHandle);
  if(read >= 0) {
    byte = (uint8_t)read;
    return true;
  }
  
  return false;
}

void I2C::initPIGPIOD() {
  if(0 != pigpio_start(NULL, NULL)) {
    std::cerr << "Failed to connect to pigpiod, is it running?" << std::endl;
    exit(-1);
  }
  
  // We want to stop the pigpio connection when the program exits
  std::atexit(pigpio_stop);
}

}