/**
 * The SensorVL6180 class initialises and controls access to
 * the SparkFun VL6180 Range sensor.
 * https://cdn.sparkfun.com/assets/learn_tutorials/2/5/7/VL6180_ApplicationNote.pdf
 */

#include "SensorVL6180.h"

#include <iostream>
#include <thread>

namespace PiWars
{


SensorVL6180::SensorVL6180() : Sensor(), I2CExternal(0x29), _initialised(false), _range(255), _rangeReader(nullptr), _rangeReaderQuit(false) {
}

SensorVL6180::~SensorVL6180() {
}

bool SensorVL6180::exists() {
  return I2C::exists();
}

bool SensorVL6180::enable() {
  if(!isEnabled()) {
    // Set the 'cached' range to max
    _range = 255;

    // Initialise the sensor
    init();

    // Create a thread to poll the range sensor, so there is always a valid
    // range ready to be read.
    _rangeReaderQuit = false;
    _rangeReader = new std::thread(rangeReader, std::ref(_rangeReaderQuit), std::ref(_range));

    // Call the base class to perform any
    // generic changes
    Sensor::enable();
  }

  return isEnabled();

}

void SensorVL6180::disable() {
  if(isEnabled()) {
    // Tell the thread to exit
    _rangeReaderQuit = false;

    // and wait for it to do so
    _rangeReader->join();
    _rangeReader = nullptr;

    Sensor::disable();
  }
}

void SensorVL6180::rangeReader(std::atomic<bool> &quit, std::atomic<uint8_t> &range) {
  I2CExternal rangeSensor(0x29);

  while(!quit.load()) {
    uint32_t attempts = 0;
    char status;
    char range_status;

    // Request a range to be sampled
    writeByte(&rangeSensor, 0x018,0x01);

    // check the status
    status = readByte(&rangeSensor, 0x04f);
    range_status = status & 0x07;

    // wait for new measurement ready status, giving up after
    // 10 attempts to avoid an infinite loop if the Sensor
    // glitches
    while (range_status != 0x04 && attempts++ < 10) {
      status = readByte(&rangeSensor, 0x04f);
      range_status = status & 0x07;
      std::this_thread::sleep_for (std::chrono::microseconds(1000));
    }

    // Read in the actual range
    range = readByte(&rangeSensor, 0x062);

    // Tell the sensor we are done
    writeByte(&rangeSensor, 0x015,0x07);

    std::this_thread::sleep_for (std::chrono::microseconds(1000));
  }
}

void SensorVL6180::init() {
  char reset;
  reset = readByte(this, 0x016);

  //if (reset == 1)
  // Currently we reinitalise the sensor every time, as it doesn't
  // always respond correctly
  {

    // Settings taken from Section 9 of the datasheet

    // Mandatory : private registers
    writeByte(this, 0x0207, 0x01);
    writeByte(this, 0x0208, 0x01);
    writeByte(this, 0x0096, 0x00);
    writeByte(this, 0x0097, 0xfd);
    writeByte(this, 0x00e3, 0x00);
    writeByte(this, 0x00e4, 0x04);
    writeByte(this, 0x00e5, 0x02);
    writeByte(this, 0x00e6, 0x01);
    writeByte(this, 0x00e7, 0x03);
    writeByte(this, 0x00f5, 0x02);
    writeByte(this, 0x00d9, 0x05);
    writeByte(this, 0x00db, 0xce);
    writeByte(this, 0x00dc, 0x03);
    writeByte(this, 0x00dd, 0xf8);
    writeByte(this, 0x009f, 0x00);
    writeByte(this, 0x00a3, 0x3c);
    writeByte(this, 0x00b7, 0x00);
    writeByte(this, 0x00bb, 0x3c);
    writeByte(this, 0x00b2, 0x09);
    writeByte(this, 0x00ca, 0x09);
    writeByte(this, 0x0198, 0x01);
    writeByte(this, 0x01b0, 0x17);
    writeByte(this, 0x01ad, 0x00);
    writeByte(this, 0x00ff, 0x05);
    writeByte(this, 0x0100, 0x05);
    writeByte(this, 0x0199, 0x05);
    writeByte(this, 0x01a6, 0x1b);
    writeByte(this, 0x01ac, 0x3e);
    writeByte(this, 0x01a7, 0x1f);
    writeByte(this, 0x0030, 0x00);

    // Recommended : Public registers - See data sheet for more detail
    writeByte(this, 0x0011, 0x10); // Enables polling for ‘New Sample ready’
                             // when measurement completes
    writeByte(this, 0x010a, 0x30); // Set the averaging sample period
                             // (compromise between lower noise and
                             // increased execution time)
    writeByte(this, 0x003f, 0x46); // Sets the light and dark gain (upper
                             // nibble). Dark gain should not be
                             // changed.
    writeByte(this, 0x0031, 0xFF); // sets the # of range measurements after
                             // which auto calibration of system is
                             // performed
    writeByte(this, 0x0040, 0x63); // Set ALS integration time to 100ms
    writeByte(this, 0x002e, 0x01); // perform a single temperature calibration
                             // of the ranging sensor

    // Optional: Public registers - See data sheet for more detail
    writeByte(this, 0x001b, 0x09); // Set default ranging inter-measurement
                             // period to 100ms
    writeByte(this, 0x003e, 0x31); // Set default ALS inter-measurement period
                             // to 500ms
    writeByte(this, 0x0014, 0x24); // Configures interrupt on ‘New Sample
                             // Ready threshold event’

    writeByte(this, 0x016, 0x00); //change fresh out of set status to 0
  }
}

void SensorVL6180::writeByte(I2C *i2c, uint16_t reg, char data) {
  char data_write[3];

  data_write[0] = (reg >> 8) & 0xFF;; // MSB of register address
  data_write[1] = reg & 0xFF; // LSB of register address
  data_write[2] = data & 0xFF;
  i2c->writeBytes(data_write, 3);

}

char SensorVL6180::readByte(I2C *i2c, uint16_t reg) {
  char data_write[2];
  char data_read[1];
  data_write[0] = (reg >> 8) & 0xFF; // MSB of register address
  data_write[1] = reg & 0xFF; // LSB of register address
  i2c->writeBytes(data_write, 2);
  i2c->readBytes(data_read, 1);

  return data_read[0];

}

}
