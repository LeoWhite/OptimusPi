/**
 * The I2C class allows for communication with I2C devices connected to the 
 * Raspberry Pi.
 */

#ifndef _PIWARS_I2C_H
#define _PIWARS_I2C_H

#include <cstdint>
#include <cstddef>

namespace PiWars {

  // Represents a single I2C device
  class I2C {
    public:
      // Initialize the I2C class to talk to the specified device 
      //
      // @param i2cAddress The address of the I2C device to communicate with
      I2C(uint8_t i2cAddress);
      virtual ~I2C();

      // Checks if the i2c device is present
      bool exists();
      
      // Write to the device
      //
      // @param bytes Pointer to the bytes to write
      // @param length Number of bytes to write
      //
      // @returns true if all bytes were written
      virtual bool writeBytes(const char *bytes, size_t length) = 0;
      
      // Read from the device
      //
      // @param buffer Pointer to the buffer to read into
      // @param length Number of bytes to read
      //
      // @returns Number of bytes read
      virtual size_t readBytes(char *buffer, size_t length) = 0;
      
      // Writes a single byte to the device
      // 
      // @param byte The byte to write
      // @returns true if the byte was written
      virtual bool writeByte(const uint8_t byte) = 0;
      
      // Reads a single byte
      //
      // @param byte Where to read the byte into
      // @returns true if the byte was successfully read
      virtual bool readByte(uint8_t &byte) = 0;
      
    protected:
      // Get the address of the I2C slave
      // @returns the devices i2c address
      uint8_t address() { return _i2cAddress; }
      
    private:
      static void initPIGPIOD(); //!< Initialise a connection to the PIGPIO daemon
      static void finalizePIGPIOD(); //!< Finalizes the connection with the PIGPIO daemon
      
      uint8_t _i2cAddress; //!< Address of the I2CDevice as returned by pigpiod
  };

  class I2CInternal : public I2C {
    public:
      // Initialize the I2C class to talk to a device that is
      // connected on the 'Internal' I2C bus. (i.e. the standard
      // /dev/i2c-1 device)
      //
      // @param i2cAddress The address of the I2C device to communicate with
      I2CInternal(uint8_t i2cAddress);
      ~I2CInternal();

      // Implementation of the APIs for the internal I2C
      bool writeBytes(const char *bytes, size_t length);      
      size_t readBytes(char *buffer, size_t length);      
      bool writeByte(const uint8_t byte);
      bool readByte(uint8_t &byte);

    private:      
      int _i2cHandle; //!< Handle used by pigpiod to perform I2C communication
  };
  
  class I2CExternal : public I2C {
    public:
      // Initialize the I2C class to talk to a device that is
      // connected on the 'External' I2C bus. (i.e. the pigpiod
      // bit-bangged bus)
      //
      // @param i2cAddress The address of the I2C device to communicate with
      I2CExternal(uint8_t i2cAddress);
      ~I2CExternal();

      // Implementation of the APIs for the internal I2C
      bool writeBytes(const char *bytes, size_t length);      
      size_t readBytes(char *buffer, size_t length);      
      bool writeByte(const uint8_t byte);
      bool readByte(uint8_t &byte);

    private:      
  };  
}

#endif