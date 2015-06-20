/* I2CRomlib.h
 * defintions for 24LC256 I2C EEPROM - 256kbit
 */
#ifndef I2CRom_h
#define I2CRom_h
#include <avr/pgmspace.h> 
#include <Arduino.h>
#define I2CRom_ADDRESS 0x50

class I2CRom{
  public:
    void write(unsigned int address, byte data);
    byte read(unsigned int address);
    I2CRom();	 
  private:

};

#endif
