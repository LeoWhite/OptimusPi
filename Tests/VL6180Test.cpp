// Example code taken from https://cdn.sparkfun.com/assets/learn_tutorials/2/5/7/VL6180_ApplicationNote.pdf
// and modified to use the pigpio
//
// Build using 'g++ -o VL6180Test  -lpthread -lpigpiod_if VL6180Test.cpp'
// Run using './VL6180Test'
// Note: The pigpiod process needs to be running first
// 
///////////////////////////////////////////////////////////////////
// Beginning of code
///////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <iostream>
#include <cstdlib>

// The pigpiod_if header file doesn't protect itself when included from
// C++, so force it to be treated as 'C' here.
extern "C" {
#include "pigpiod_if.h"
}

#define addr (0x29) // I²C address of VL6180X
 
int i2cHandle; // i2c handle returned by pigpiod

///////////////////////////////////////////////////////////////////
// Split 16-bit register address into two bytes and write
// the address + data via I²C
///////////////////////////////////////////////////////////////////
void WriteByte(uint16_t reg,char data) {
  char data_write[3];
  data_write[0] = (reg >> 8) & 0xFF;; // MSB of register address
  data_write[1] = reg & 0xFF; // LSB of register address
  data_write[2] = data & 0xFF;
  i2c_write_device(i2cHandle, data_write, 3);
}

///////////////////////////////////////////////////////////////////
// Split 16-bit register address into two bytes and write
// required register address to VL6180X and read the data back
///////////////////////////////////////////////////////////////////
char ReadByte(uint16_t reg) {
  char data_write[2];
  char data_read[1];
  data_write[0] = (reg >> 8) & 0xFF; // MSB of register address
  data_write[1] = reg & 0xFF; // LSB of register address
  i2c_write_device(i2cHandle, data_write, 2);
  i2c_read_device(i2cHandle, data_read, 1);
  
  return data_read[0];
}

///////////////////////////////////////////////////////////////////
// load settings
///////////////////////////////////////////////////////////////////
int VL6180X_Init() {
  char reset;
  reset = ReadByte(0x016);
  
  if (reset==1)
  { // check to see has it be Initialised already
      
    // Settings taken from Section 9 of the datasheet      

    // Mandatory : private registers
    WriteByte(0x0207, 0x01);
    WriteByte(0x0208, 0x01);
    WriteByte(0x0096, 0x00);
    WriteByte(0x0097, 0xfd);
    WriteByte(0x00e3, 0x00);
    WriteByte(0x00e4, 0x04);
    WriteByte(0x00e5, 0x02);
    WriteByte(0x00e6, 0x01);
    WriteByte(0x00e7, 0x03);
    WriteByte(0x00f5, 0x02);
    WriteByte(0x00d9, 0x05);
    WriteByte(0x00db, 0xce);
    WriteByte(0x00dc, 0x03);
    WriteByte(0x00dd, 0xf8);
    WriteByte(0x009f, 0x00);
    WriteByte(0x00a3, 0x3c);
    WriteByte(0x00b7, 0x00);
    WriteByte(0x00bb, 0x3c);
    WriteByte(0x00b2, 0x09);
    WriteByte(0x00ca, 0x09);
    WriteByte(0x0198, 0x01);
    WriteByte(0x01b0, 0x17);
    WriteByte(0x01ad, 0x00);
    WriteByte(0x00ff, 0x05);
    WriteByte(0x0100, 0x05);
    WriteByte(0x0199, 0x05);
    WriteByte(0x01a6, 0x1b);
    WriteByte(0x01ac, 0x3e);
    WriteByte(0x01a7, 0x1f);
    WriteByte(0x0030, 0x00);
    
    // Recommended : Public registers - See data sheet for more detail
    WriteByte(0x0011, 0x10); // Enables polling for ‘New Sample ready’
                             // when measurement completes
    WriteByte(0x010a, 0x30); // Set the averaging sample period
                             // (compromise between lower noise and
                             // increased execution time)
    WriteByte(0x003f, 0x46); // Sets the light and dark gain (upper
                             // nibble). Dark gain should not be
                             // changed.
    WriteByte(0x0031, 0xFF); // sets the # of range measurements after
                             // which auto calibration of system is
                             // performed
    WriteByte(0x0040, 0x63); // Set ALS integration time to 100ms
    WriteByte(0x002e, 0x01); // perform a single temperature calibration
                             // of the ranging sensor

    // Optional: Public registers - See data sheet for more detail
    WriteByte(0x001b, 0x09); // Set default ranging inter-measurement
                             // period to 100ms
    WriteByte(0x003e, 0x31); // Set default ALS inter-measurement period
                             // to 500ms
    WriteByte(0x0014, 0x24); // Configures interrupt on ‘New Sample
                             // Ready threshold event’ 
   
    WriteByte(0x016, 0x00); //change fresh out of set status to 0
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
// Start a range measurement in single shot mode
///////////////////////////////////////////////////////////////////
int VL6180X_Start_Range() {
  WriteByte(0x018,0x01);
  return 0;
}
///////////////////////////////////////////////////////////////////
// poll for new sample ready ready
///////////////////////////////////////////////////////////////////
int VL6180X_Poll_Range() {
  char status;
  char range_status;
  
  // check the status
  status = ReadByte(0x04f);
  range_status = status & 0x07;
  
  // wait for new measurement ready status
  while (range_status != 0x04) {
    status = ReadByte(0x04f);
    range_status = status & 0x07;
    usleep(1000); // (can be removed)
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
// Read range result (mm)
///////////////////////////////////////////////////////////////////
int VL6180X_Read_Range() {
  int range;
  range=ReadByte(0x062);
  return range;
}

///////////////////////////////////////////////////////////////////
// clear interrupts
///////////////////////////////////////////////////////////////////
int VL6180X_Clear_Interrupts() {
  WriteByte(0x015,0x07);
  return 0;
}

///////////////////////////////////////////////////////////////////
// Main Program loop
///////////////////////////////////////////////////////////////////
int main()
{
  int range;
   
  if(0 != pigpio_start(NULL, NULL)) {
    std::cerr << "Failed to connect to pigpiod, is it running?" << std::endl;
    exit(-1);
  }
   
  i2cHandle = i2c_open(1, addr, 0);
  
  // load settings onto VL6180X
  VL6180X_Init();
  
  while(1) {
    // start single range measurement
    VL6180X_Start_Range();
    
    // poll the VL6180X till new sample ready
    VL6180X_Poll_Range();
    
    // read range result
    range = VL6180X_Read_Range();
    
    // clear the interrupt on VL6180X
    VL6180X_Clear_Interrupts();
    
    // Output range
    std::cout << "range : " << range << "mm" << std::endl;
    
    // Wait before taking the next reading
    sleep(1);
  }
}
