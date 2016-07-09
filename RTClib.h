/* RTClib.h
 * defintions for DS1307 RTC 
 */
#ifndef RTClib_h
#define RTClib_h
#include <avr/pgmspace.h> 
#include <Arduino.h>
#define DS1307_ADDRESS 0x68
#define SECONDS_PER_DAY 86400
#define SECONDS_FROM_1970_TO_2000 946684800
static const uint8_t daysInMonth [] PROGMEM  = {31,28,31,30,31,30,31,31,30,31,30,31};

class RTClib{
  public:
	  RTClib();
	  void setTime(String _serIn);
	  String getDate();
	  String getTime();	
    void getTime(String &hour, String &minute);
	  uint8_t getSec()     {return ss;}
          uint8_t getMinute()  {return mm;}
          uint8_t getHour()    {return hh;}
          uint8_t getDay()     {return d;}
          uint8_t getMonth()   {return m;}
          uint8_t getYear()   {return y;}
      	  boolean timeChanged();    //True if pmm != mm 
	  int date2days(uint8_t d, uint8_t m, uint8_t y);
	  uint16_t dateDiff(uint8_t day, uint8_t month, uint8_t year);
	  uint16_t timeDiff(uint8_t minutes, uint8_t hours);
  private:
	  String serIn; //Serial data input
	  String tempStr; //temp string from serIn
	  char cArray[5]; //char Array for string to integer convertion
	  byte lastData; //to keep track of my current position in serIn String.
	  uint8_t y;
	  uint8_t m, d, hh, mm, ss;  
	  uint8_t pmm;					//pmm = prevous minute
	  uint8_t bcd2dec(uint8_t val);
	  uint8_t dec2bcd(uint8_t val);
	  String formatNumber(uint8_t number);
	  void now();
	  void writeTime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss); //write time to RTC chip
};

#endif
