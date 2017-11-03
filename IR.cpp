/******************************************************************
*Writen for DS1307 RTC 
*/
#include "IR.h"

int IRpin;

IR::IR(int _IRpin){ //constructor
  IRpin = _IRpin;
  auxtek = {{4450, 4500}, {{580,580},{580,1700}}, 16, 16, false, {0,0}};
  deviation = 200;
  maximumDelay = 5000;
  IRAvailable = false;
  DDRD &= ~(B00000001 << IRpin); //Set IRpin as input
  resetTransmission();
}



/******************************************************************
 *  Get bit from IR remote
 */
void IR::receiveBit(){
   if (micros() - lastInterrupt < maximumDelay){
      if ((PIND & (1 << IRpin))){                             //IR pin HIGH (Low part of the bit received)
        receivedPulse[LOW] = micros() - lastInterrupt;
      }
      else if (!(PIND & (1 << IRpin)) && receivedPulse[LOW] > 0){ //IR pin LOW (Low & High part of the bit received)
        receivedPulse[HIGH] = micros() - lastInterrupt;
        writeBit();
      }
      else{                                                   //HIGH pulse before LOW 
        resetTransmission();
      }
   }
   else{                                                      //Delay too long
    resetTransmission();
   }
   
   lastInterrupt = micros();
}


/******************************************************************
 *  Write bit to tmp variable
 */
void IR::writeBit(){
  if (identifiedProtocol != NONE){
    if (bitNumber < 16) address |= (decodeBit(receivedPulse, auxtek.data) << (15 - bitNumber));  
    else command |= (decodeBit(receivedPulse, auxtek.data) << (31 - bitNumber));  

    bitNumber++;
    if (bitNumber == 32) IRAvailable = true;
    resetBit();
  }
  else{
    identifyProtocol();
  }
}


/******************************************************************
 *  Identify IR protocol
 */
void IR::identifyProtocol(){
  if (isEqual(receivedPulse[LOW], auxtek.header[LOW]) && isEqual(receivedPulse[HIGH], auxtek.header[HIGH])){
    identifiedProtocol = AUXTEK;
    resetBit();
  }
  else{
    resetTransmission();
  }
}



/******************************************************************
 *  Decode Bit
 */
boolean IR::decodeBit(unsigned long receivedPulse[2], uint16_t data[2][2]){
  if (isEqual(receivedPulse[LOW], data[LOW][LOW]) && isEqual(receivedPulse[HIGH], data[LOW][HIGH])){
    return false;
  }
  else{
    return true;
  }
}



/******************************************************************
 *  Compare received pulse with protocol
 */
boolean IR::isEqual(unsigned volatile long receivedPulse, uint16_t compare){
  int16_t temp = receivedPulse - compare;
  return (abs(temp) < deviation); 
}


/******************************************************************
 *  Reset Transmission (On error or received msg)
 */
void IR::resetTransmission(){
   bitNumber = 0;
   resetBit();
   identifiedProtocol = NONE;
   address = 0;
   command = 0;
}


/******************************************************************
 *  Reset Bit 
 */
void IR::resetBit(){
   receivedPulse[LOW] = 0;
   receivedPulse[HIGH] = 0;
}

/******************************************************************
 *  Print Decoded data
 */
void IR::printDecodedData(){
  Serial.println(address , BIN);
  Serial.println(command , BIN);
  Serial.println();
}


/******************************************************************
 *	Check for available IR transmission
 */
boolean IR::available(){
  return IRAvailable;
}



/******************************************************************
 *	Get command from IR remote
 */
char IR::getCommand(){
  IRAvailable = false;

  //printDecodedData();
  
  switch(command){
    case 0x40BF:
      return 'P';    //Power (On/Off)
      break;  
    case 0xE01F:
      return 'L';    //Louder
      break;
    case 0xD02F:
      return 'Q';     //Quiet
      break;
    case 0x48B7:
      return '+';
      break;
    case 0x08F7:
      return '-';
      break;
    default:
      return 'E';    //Error
  }
}
