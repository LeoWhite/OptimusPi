/**
 * The I2C class allows for communication with I2C devices connected to the 
 * Raspberry Pi.
 *
 * This is currently implemented using the pigpio library
 */

#include "I2C.h"
#include <mutex>

// The pigpiod_if header file doesn't protect itself when included from
// C++, so force it to be treated as 'C' here.
extern "C" {
#include "pigpiod_if.h"
}

#include <iostream>
#include <sstream>
#include "string.h"

namespace PiWars
{

// The GPIO pins to use for the 'External' I2C bus
const static uint32_t SDA_PIN = 5;
const static uint32_t SDL_PIN = 6;


std::once_flag initPIGPIODFlag;
  
I2C::I2C(uint8_t i2cAddress) : _i2cAddress(i2cAddress) {
  // We need to initialise pigpiod once, regardless of how many 
  // i2c connections we establish
  std::call_once(initPIGPIODFlag, initPIGPIOD);
}

I2C::~I2C() {
}

bool I2C::exists() {
  return writeByte(0x00);
}

void I2C::initPIGPIOD() {
  if(0 != pigpio_start(NULL, NULL)) {
    std::cerr << "Failed to connect to pigpiod, is it running?" << std::endl;
    exit(-1);
  }

  // Create a 'bit bang' variant
  if(0 != bb_i2c_open(SDA_PIN, SDL_PIN, 100000)) {
    std::cerr << "Failed to open bit bang port" << std::endl;
    exit(-1);
  }

  
  // We want to stop the pigpio connection when the program exits
  std::atexit(finalizePIGPIOD);
}

void I2C::finalizePIGPIOD() {
  // IMPROVE: We should also make sure that any i2c handles are closed
  bb_i2c_close(SDA_PIN);
  pigpio_stop();
}


I2CInternal::I2CInternal(uint8_t i2cAddress) : I2C(i2cAddress), _i2cHandle(-1) {
  _i2cHandle = i2c_open(1, address(), 0);
}

I2CInternal::~I2CInternal() {
  i2c_close(_i2cHandle);
}

bool I2CInternal::writeBytes(const char *bytes, size_t length) {
  bool result = false;
  
  if(i2c_write_device(_i2cHandle, (char *)bytes, length) >= 0 ) {
    result = true;
  }  
  else {
    std::cerr << "Failed to write I2C bytes!" << std::endl;
  }
    
  return result;
}

size_t I2CInternal::readBytes(char *buffer, size_t length) {
  int read;
  
  read = i2c_read_device(_i2cHandle, buffer, length);
  
  if(read < 0) {
    // Set to 0 for now
    // IMPROVE: Should we throw an error?
    read = 0;
  }
  
  return read;
}

bool I2CInternal::writeByte(const uint8_t byte) {  
  if(i2c_write_byte(_i2cHandle, byte) >= 0) {
    return true;
  }
  else {
    std::cerr << "Failed to write I2C byte!" << std::endl;
  }
  
  return false;
}

bool I2CInternal::readByte(uint8_t &byte) {
  int read;
  
  read = i2c_read_byte(_i2cHandle);
  if(read >= 0) {
    byte = (uint8_t)read;
    return true;
  }
  
  return false;
}

I2CExternal::I2CExternal(uint8_t i2cAddress) : I2C(i2cAddress) {
}

I2CExternal::~I2CExternal() {
}

bool I2CExternal::writeByte(const uint8_t byte) {  
  bool success = false;
  
  // Set Address, address, Start, Write, 1 byte, Stop, End 
  char command[] = { 0x04, address(), 0x02, 0x07, 0x01, byte, 0x03, 0x00};
  
  if(0 == bb_i2c_zip(SDA_PIN, command, sizeof(command), NULL, 0)) {
    success = true;
  }

  return success;
}

bool I2CExternal::readByte(uint8_t &byte) {
  bool read = false;
  
  // Set Address, address, Start, Read, 1 byte, Stop, End 
  char command[] = { 0x04, address(), 0x02, 0x06, 0x01, 0x03, 0x00};
  
  if(1 == bb_i2c_zip(SDA_PIN, command, sizeof(command), (char *)&byte, 1)) {
    read = true;
  }
  
  return read;
}
  
bool I2CExternal::writeBytes(const char *bytes, size_t length) {
  bool success = false;
  char command[64];
  int commandLength = 0;
  
  // Is the length too long?
  if(length > 32) {
    return false;
  }
  
  command[commandLength++] = 0x04; // Set address
  command[commandLength++] = address(); // The address
  command[commandLength++] = 0x02; // Start
  command[commandLength++] = 0x07; // Write
  command[commandLength++] = length; // Number of bytes
  
  // Copy the bytes to send
  memcpy(&command[commandLength], bytes,length);
  commandLength += length;
  
  command[commandLength++] = 0x03; // Stop
  command[commandLength++] = 0x00; // End
  
  if(0 == bb_i2c_zip(SDA_PIN, command, commandLength, NULL, 0)) {
    success = true;
  }

  return success;
}

size_t I2CExternal::readBytes(char *buffer, size_t length) {
  // Set Address, address, Start, Read, length, Stop, End 
  char command[] = { 0x04, address(), 0x02, 0x06, (char)length, 0x03, 0x00};
  size_t read = 0;
  
  if(length > 0xFF) {
    return false;
  }
  
  // Check if we read all the requested bytes
  if(length == bb_i2c_zip(SDA_PIN, command, sizeof(command), buffer, length)) {
    read = length;
  }
  
  return read;
}

}

