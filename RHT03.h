/* RHT03lib.h
 * defintions for RHT03 Temperature&Humidity Sensor 
 */
#ifndef RHT03_h
#define RHT03_h
#include <avr/pgmspace.h> 
#include <Arduino.h>

class RHT03{
  public:
	  RHT03(uint8_t _RHT03_Pin);
	  int getTemperature();
	  int getHumidity();	  
  private:
	  uint8_t RHT03_Pin;
	  uint8_t RHT03_Data[5];
	  int temperature;
	  uint8_t humidity;
	  void	  updateTH();
	  uint8_t Read_RHT03_Data();
};

#endif
