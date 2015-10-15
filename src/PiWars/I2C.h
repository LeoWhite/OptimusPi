/**
 * The I2C class allows for communication with I2C devices connected to the 
 * Raspberry Pi. This includes 
 */

#ifndef _PIWARS_I2C_H
#define _PIWARS_I2C_H

#include <cstdint>
#include <cstddef>

namespace PiWars {

  // Represents a single I2C device
  class I2C {
    public:
      // Initialize the I2C class to talk to a device
      //
      // @param i2cAddress The address of the I2C device to communicate with
      I2C(uint8_t i2cAddress);
      ~I2C();

      // Checks if the i2c device is present
      bool exists();
      
      // Write to the device
      //
      // @param bytes Pointer to the bytes to write
      // @param length Number of bytes to write
      //
      // @returns true if all bytes were written
      bool writeBytes(const char *bytes, size_t length);
      
      // Read from the device
      //
      // @param buffer Pointer to the buffer to write to
      // @param length Number of bytes to read
      //
      // @returns Number of bytes read
      size_t readBytes(char *buffer, size_t length);
      
    private:
      // Initialze a connection to the PIGPIO daemon
      static void initPIGPIOD();
      
      uint8_t _i2cAddress; //!< Address of the I2CDevice
      
      int _i2cHandle; //!< Handle used by pigpio to perform I2C communication
  };

}

#endif