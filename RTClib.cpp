/******************************************************************
*Writen for DS1307 RTC 
*/
#include "RTClib.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\Wire\Wire.h"

RTClib::RTClib(){ //constructor
	Wire.begin();
}

/******************************************************************
 *	Update to current time
 */
void RTClib::now(){
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0);
  Wire.endTransmission(); 
  Wire.requestFrom(DS1307_ADDRESS,7);
  ss = bcd2dec(Wire.read() & 0x7F);
  mm = bcd2dec(Wire.read());
  hh = bcd2dec(Wire.read());
  Wire.read();
  d = bcd2dec(Wire.read());
  m = bcd2dec(Wire.read());
  y = bcd2dec(Wire.read()) + 2000;
}

/******************************************************************
 *	Check if current minute changed
 */
boolean RTClib::timeChanged(){
	now();
	if (pmm != mm){
		pmm = mm;
		return true;
	}
	else{
		return false;
	}
}


/******************************************************************
 *	Read current date value from RTC chip
 */
String RTClib::getDate(){
	 RTClib::now();
	 return (formatNumber(d) + "/" + formatNumber(m));
}


/******************************************************************
 *	Read current time value from RTC chip
 */
String RTClib::getTime(){
	RTClib::now();
	return (formatNumber(hh) + " " + formatNumber(mm));
}

/******************************************************************
 *	Set time from string. 
 *      Time Format: "c: dd/mm/yyyy hh:mm:ss PM" <-(or AM) 
 *      9600 baud recommended!!!
 */
void RTClib::setTime(String _serIn){
	serIn = _serIn;
	lastData = 3;	 //Starts from 3 because my time data starts after "c: "

    tempStr = serIn.substring(3, serIn.indexOf("/"));
    lastData = serIn.indexOf("/") + 1;
    tempStr.toCharArray(cArray, tempStr.length() + 1);
	d = atoi(cArray);
	if (d > 31 || d <= 0){
			Serial.println("Day Error");
			return;
	}  

    
    tempStr = serIn.substring(lastData, serIn.indexOf("/", lastData));
	lastData = serIn.indexOf("/", lastData) + 1;
    tempStr.toCharArray(cArray, tempStr.length() + 1);
	m = atoi(cArray);
	if (m > 12 || m <= 0){
			Serial.println("Month Error");
			return;
	}  
     
    tempStr = serIn.substring(lastData, lastData + 4);
    lastData += 5;
    tempStr.toCharArray(cArray, tempStr.length() + 1);
    y = atoi(cArray);
	if (y > 2099 || y < 2012){
			Serial.println("Year Error");
			return;
	}  
   
    tempStr = serIn.substring(lastData, serIn.indexOf(":", lastData));
    lastData = serIn.indexOf(":", lastData) + 1;
    tempStr.toCharArray(cArray, tempStr.length() + 1);
    hh = atoi(cArray);
	if (hh > 24 || hh < 0){
			Serial.println("Hour Error");
			return;
	}  
    
    tempStr = serIn.substring(lastData, serIn.indexOf(":", lastData));
    lastData = serIn.indexOf(":", lastData) + 1;
    tempStr.toCharArray(cArray, tempStr.length() + 1);
    mm = atoi(cArray);
	if (mm > 60 || mm < 0){
			Serial.println("Mimute Error");
			return;
	}  
    
    tempStr = serIn.substring(lastData, lastData + 2);
    lastData += 3;
    tempStr.toCharArray(cArray, tempStr.length() + 1);
    ss = atoi(cArray);
	if (ss > 60 || ss < 0){
			Serial.println("Second Error");
			return;
	}  
    
    tempStr = serIn.substring(lastData, lastData + 2);
    lastData = 3; //to prepare for next String 
    if (tempStr == "PM") hh += 12;

	RTClib::writeTime(y, m, d, hh, mm, ss);
}


/******************************************************************
 *	Write time to RTC Chip
 */
 void RTClib::writeTime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss){
	Wire.beginTransmission(DS1307_ADDRESS);
	Wire.write(0);
	Wire.write(dec2bcd(ss));			//sec
	Wire.write(dec2bcd(mm));			//min
	Wire.write(dec2bcd(hh));			//hour
	Wire.write(dec2bcd(0));			//day of week
	Wire.write(dec2bcd(d));			//day
	Wire.write(dec2bcd(m));			//month
	Wire.write(dec2bcd(y - 2000));	//year
	Wire.write(0);
	Wire.endTransmission(); 
 }

/******************************************************************
 *	Number of minutes between now and some time in future
 */
uint16_t RTClib::timeDiff(uint8_t minutes, uint8_t hours){
 uint8_t carry;
 if (minutes < mm){
	minutes += 60; 
	carry = 60; 
 }
 if (hours < hh){
	 hours += 24;
 }

 return minutes - mm + 60 * (hours - hh) - carry;  
}



/******************************************************************
 *	Number of days from now to some date
 */
uint16_t RTClib::dateDiff(uint8_t day, uint8_t month, uint16_t year){
	return date2days(day, month, year) - date2days(d, m, y);
}


/******************************************************************
 *	Number of days since 2000/01/01, valid for 2001....2099
 */
uint16_t RTClib::date2days(uint8_t day, uint8_t month, uint16_t year){
	year -= 2000;
	uint16_t days = day; 
	for (uint8_t i = 1; i < month; i++)
		days += pgm_read_byte(daysInMonth + i - 1);
	if (month > 2 && year % 4 == 0)
		days++;

	return days + 365 * year + (year + 3) / 4 - 1; 
}


/******************************************************************
 * 1. Add zero to time or date represented by Single digit (18:4 -> 18:04, 1:2 -> 01:02)
 * 2. Change time or date format from uint8_t to String 
 */
String RTClib::formatNumber(uint8_t number){
	String value;
  if (number < 10){
    value = "0" + String(number,DEC);
  }
  else{
    value = String(number,DEC);
  }
  return value;
}

/******************************************************************
 *	Convert BCD to Binary
 */
uint8_t RTClib::bcd2dec(uint8_t val){
  return val - 6 * (val >> 4);  
}
/******************************************************************
 *	Convert Binary to BCD
 */
uint8_t RTClib::dec2bcd(uint8_t val){
  return val + 6 * (val / 10); 
}

