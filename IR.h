/* IR.h
 *
 */
#ifndef IR_h
#define IR_h
#include <avr/pgmspace.h> 
#include <Arduino.h>


class IR{
  public:
	  IR(int _IRpin);
          char receive();
          boolean available();      
  private:
          word getData();
          int IRpin;
          word address;
          word command;
          
};

#endif
