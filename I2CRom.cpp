/******************************************************************
*Writen for 24LC256 I2C EEPROM - 256kbit
*/
#include "I2CRom.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\Wire\Wire.h"

I2CRom::I2CRom(){ //constructor
	Wire.begin();
}


/******************************************************************
 *	Write data to EEPROM
 */
void I2CRom::write(unsigned int address, byte data){ //constructor
  Wire.beginTransmission(I2CRom_ADDRESS);
  Wire.write(highByte(address));
  Wire.write(lowByte(address)); 
  Wire.write(data); 
  Wire.endTransmission();
  delay(10);
}


/******************************************************************
 *	Read data from EEPROM
 */
byte I2CRom::read(unsigned int address){
  byte data = 0xFF;
  Wire.beginTransmission(I2CRom_ADDRESS);
  Wire.write(highByte(address));
  Wire.write(lowByte(address)); 
  Wire.endTransmission();
  Wire.requestFrom(I2CRom_ADDRESS,1);
  if(Wire.available()) data = Wire.read();
  return data;
}

