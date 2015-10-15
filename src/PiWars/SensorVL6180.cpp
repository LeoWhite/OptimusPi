#include "SensorVL6180.h"

#include <iostream>
#include <thread>

namespace PiWars
{

  
SensorVL6180::SensorVL6180() : Sensor(), I2C(0x29), _initialised(false) {
  // If the sensor exists, then init it now
  if(exists()) {
    init();
  }
}

SensorVL6180::~SensorVL6180() {
}

bool SensorVL6180::exists() {
  return I2C::exists();
}

uint8_t SensorVL6180::range() {
  // Start the range
  writeByte(0x018,0x01);

  char status;
  char range_status;
  
  // check the status
  status = readByte(0x04f);
  range_status = status & 0x07;
  
  // wait for new measurement ready status
  while (range_status != 0x04) {
    status = readByte(0x04f);
    range_status = status & 0x07;
    std::this_thread::sleep_for (std::chrono::microseconds(1000));
  }
  
  int range;
  range=readByte(0x062);

  writeByte(0x015,0x07);

  return (uint8_t)range;    
}

void SensorVL6180::init() {
  char reset;
  reset = readByte(0x016);
  
  if (reset == 1)
  { // check to see has it be Initialised already
      
    // Settings taken from Section 9 of the datasheet      

    // Mandatory : private registers
    writeByte(0x0207, 0x01);
    writeByte(0x0208, 0x01);
    writeByte(0x0096, 0x00);
    writeByte(0x0097, 0xfd);
    writeByte(0x00e3, 0x00);
    writeByte(0x00e4, 0x04);
    writeByte(0x00e5, 0x02);
    writeByte(0x00e6, 0x01);
    writeByte(0x00e7, 0x03);
    writeByte(0x00f5, 0x02);
    writeByte(0x00d9, 0x05);
    writeByte(0x00db, 0xce);
    writeByte(0x00dc, 0x03);
    writeByte(0x00dd, 0xf8);
    writeByte(0x009f, 0x00);
    writeByte(0x00a3, 0x3c);
    writeByte(0x00b7, 0x00);
    writeByte(0x00bb, 0x3c);
    writeByte(0x00b2, 0x09);
    writeByte(0x00ca, 0x09);
    writeByte(0x0198, 0x01);
    writeByte(0x01b0, 0x17);
    writeByte(0x01ad, 0x00);
    writeByte(0x00ff, 0x05);
    writeByte(0x0100, 0x05);
    writeByte(0x0199, 0x05);
    writeByte(0x01a6, 0x1b);
    writeByte(0x01ac, 0x3e);
    writeByte(0x01a7, 0x1f);
    writeByte(0x0030, 0x00);
    
    // Recommended : Public registers - See data sheet for more detail
    writeByte(0x0011, 0x10); // Enables polling for ‘New Sample ready’
                             // when measurement completes
    writeByte(0x010a, 0x30); // Set the averaging sample period
                             // (compromise between lower noise and
                             // increased execution time)
    writeByte(0x003f, 0x46); // Sets the light and dark gain (upper
                             // nibble). Dark gain should not be
                             // changed.
    writeByte(0x0031, 0xFF); // sets the # of range measurements after
                             // which auto calibration of system is
                             // performed
    writeByte(0x0040, 0x63); // Set ALS integration time to 100ms
    writeByte(0x002e, 0x01); // perform a single temperature calibration
                             // of the ranging sensor

    // Optional: Public registers - See data sheet for more detail
    writeByte(0x001b, 0x09); // Set default ranging inter-measurement
                             // period to 100ms
    writeByte(0x003e, 0x31); // Set default ALS inter-measurement period
                             // to 500ms
    writeByte(0x0014, 0x24); // Configures interrupt on ‘New Sample
                             // Ready threshold event’ 
   
    writeByte(0x016, 0x00); //change fresh out of set status to 0
  }
}

void SensorVL6180::writeByte(uint16_t reg, char data) {
  char data_write[3];

  data_write[0] = (reg >> 8) & 0xFF;; // MSB of register address
  data_write[1] = reg & 0xFF; // LSB of register address
  data_write[2] = data & 0xFF;
  writeBytes(data_write, 3);
    
}

char SensorVL6180::readByte(uint16_t reg) {
  char data_write[2];
  char data_read[1];
  data_write[0] = (reg >> 8) & 0xFF; // MSB of register address
  data_write[1] = reg & 0xFF; // LSB of register address
  writeBytes(data_write, 2);
  readBytes(data_read, 1);
  
  return data_read[0];
  
}

}
