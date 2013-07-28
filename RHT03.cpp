/******************************************************************
 *Writen for RHT03 Temperature & Humidity sensor
 */
#include "RHT03.h"

RHT03::RHT03(uint8_t _RHT03_Pin){ //constructor
  RHT03_Pin = _RHT03_Pin;
  DDRD |= (1 << RHT03_Pin);    //RHT03Pin as output  
  PORTD |= (1 << RHT03_Pin);    //RH03Pin HIGH   
}

/******************************************************************
 *	Update current temperature & humidity
 */
void RHT03::updateTH(){
  for (int x = 0; x < 5; x++){
    RHT03_Data[x] = 0;
  }

  PORTD &= ~(1 << RHT03_Pin); //RHT03Pin LOW
  delay(30);
  PORTD |= (1 << RHT03_Pin);  //RHT03Pin HIGH
  delayMicroseconds(40);
  DDRD &= ~(1 << RHT03_Pin);  //RHT03Pin as input
  delayMicroseconds(40);

  //Sensor response check
  if (PIND & (1 << RHT03_Pin)){
    Serial.println("RHT03: Response Check 1 Error"); 
    return;
  }
  delayMicroseconds(80);
  if (!(PIND & (1 << RHT03_Pin))){
    Serial.println("RHT03: Response Check 2 Error"); 
    return;
  }

  delayMicroseconds(80);  
  for(int i = 0; i < 5; i++){
    RHT03_Data[i] = Read_RHT03_Data();
  }

  DDRD |= (1 << RHT03_Pin);    //RHT03Pin as output  
  PORTD |= (1 << RHT03_Pin);    //RH03Pin HIGH   


  //Check Sum, sometimes check sum bigger than 8 bit so we multiply it by B11111111
  if (RHT03_Data[4] != ((RHT03_Data[0] + RHT03_Data[1] + RHT03_Data[2] + RHT03_Data[3]) & 0xFF)){
    Serial.println(RHT03_Data[0] + RHT03_Data[1] + RHT03_Data[2] + RHT03_Data[3], BIN);
    return;
  } 
  else{
    humidity = (((RHT03_Data[0] << 8) | RHT03_Data[1]) / 10);
    temperature = (((RHT03_Data[2] << 8) | RHT03_Data[3]) / 10);
  }
}

uint8_t RHT03::Read_RHT03_Data(){
  uint8_t result = 0;

  for(int i = 0; i < 8; i++){
    while(!(PIND & (1 << RHT03_Pin))); //wait 40usec
    delayMicroseconds(40);

    if(PIND & (1 << RHT03_Pin)){  //if pin still '1' after 40usec that it's '1'
      result |= (1 << (7 - i));
    }
    while(PIND & (1 << RHT03_Pin));    //wait for next bit
  }
  return result;
}

String RHT03::getHumidity(){
  updateTH();
  return("H:" + String(humidity, DEC) + "%");
}

String RHT03::getTemperature(){
  updateTH();

  return("T:" + String(temperature, DEC) + "C");
}
