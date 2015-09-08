/******************************************************************
*Writen for DS1307 RTC 
*/
#include "IR.h"

int IRpin;

IR::IR(int _IRpin){ //constructor
  IRpin = _IRpin;
}

/******************************************************************
 *	Check for available IR transmission
 */
boolean IR::available(){
  static unsigned long lastReceived;
  
  if(millis() - lastReceived < 300) return false;  //Need gap of 300 milisec
  
  if(PIND & (1 << IRpin)) return false;            //If IR pin = '0' -> new transmission
  lastReceived = millis();
  return true;
}

/******************************************************************
 *	Get command from IR remote
 */
char IR::receive(){
  static unsigned long receiveStart;
  
  delay(11);   //Ignore Junk
  receiveStart = millis();
  while((PIND & (1 << IRpin))&& ((millis() - receiveStart) < 6));
    
  address = getData();
  command = getData();
  
  switch(command){
    case 0x3BC4:
      return 'P';    //Power (On/Off)
      break;  
    case 0xA35C:
      return 'L';    //Louder
      break;
    case 0x11EE:
      return 'Q';     //Quiet
      break;
    case 0x18E7:
      return '+';
      break;
    case 0x629D:
      return '-';
      break;
    default:
      return 'E';    //Error
  }
}
/******************************************************************
 *	Receive 16 bits from IR remote
 */
word IR::getData(){
  boolean bitValue;
  unsigned long startOfBit;
  int bitLength;
  word data = 0;
  
  for(int i = 0; i < 16; i++){
    
  startOfBit = micros();
  
  while(!(PIND & (1 << IRpin)) && (micros() - startOfBit) < 3000);
  while((PIND & (1 << IRpin)) && (micros() - startOfBit) < 3000);
  
  bitLength = micros() - startOfBit;
  
  if(bitLength < 1250 && bitLength > 1050) bitValue = 0;
  else if(bitLength < 2350 && bitLength > 2150) bitValue = 1;
  else{
   //  Serial.print("  Too long/short command at bit: ");
   //  Serial.println(i);
    return 0;
  }
    
  data |= (bitValue << (15 - i));  
}  
  return data;
}
