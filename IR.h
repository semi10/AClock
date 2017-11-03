/* IR.h
 *
 */
#ifndef IR_h
#define IR_h
#include <avr/pgmspace.h> 
#include <Arduino.h>


class IR{

  typedef struct {
    uint16_t header[2];   //[Length in '0'/'1']
    uint16_t data[2][2];  //[Logic '0'/'1'][Length in '0'/'1'];
    uint8_t addressLength;
    uint8_t commandLenght;
    bool cmdRepeat;
    uint16_t cmdRepeatLength[2]; 
  } IRprotocol;

  enum Protocol{ NONE, AUXTEK, CARMP3};

  
  public:
	  IR(int _IRpin);
          void receiveBit();
          boolean available();
          boolean IRAvailable;
          char getCommand();      
  private:
          int IRpin;
          word volatile address;
          word volatile command;
          unsigned volatile long lastInterrupt;
          Protocol volatile identifiedProtocol;
          uint8_t volatile bitNumber;
          unsigned volatile long receivedPulse[2];
          IRprotocol auxtek;
          uint16_t deviation;
          uint16_t maximumDelay;

          void writeBit();
          void identifyProtocol();
          void printDecodedData();
          void resetBit();
          boolean decodeBit(unsigned long receivedPulse[2], uint16_t data[2][2]);
          boolean isEqual(unsigned volatile long, uint16_t);
          void resetTransmission();
          
          
};

#endif
