/******************************************************************
 * Nov, 2008 by Bill Westfield ("WestfW")
 * Copyrighted and distributed under the terms of the Berkely license
 * (copy freely, but include this notice of original author.)
 *
 * May, 2011 edited by Semion Faifman for SURE electronics 32X16 RG Bicolor LED Dot Matrix Unit Board 
 */
#include "ht1632c.h"                     // Holtek LED driver by WestFW - updated to HT1632C by Nick Hall
#include <avr/pgmspace.h>                // Enable data to be stored in Flash Mem as well as SRAM              
#include "NewFont.h"                     // Font library
#include <Arduino.h>
   
ht1632c::ht1632c(byte ht1632_cs, byte ht1632_clk, byte ht1632_wrclk, byte ht1632_data){
	_ht1632_cs = ht1632_cs;			//Chip select signal input
	_ht1632_clk = ht1632_clk;		//Chip select clock signal input
	_ht1632_wrclk = ht1632_wrclk;	        //Write data clock input
	_ht1632_data = ht1632_data;		//Data input

  pinMode(_ht1632_cs, OUTPUT);
  pinMode(_ht1632_clk, OUTPUT);
  pinMode(_ht1632_wrclk, OUTPUT);
  pinMode(_ht1632_data, OUTPUT);

  for (byte c = 0; c < 4; c++){  
    sendCmd(c, HT1632_CMD_SYSON);    // System on 
    sendCmd(c, HT1632_CMD_LEDON);    // LEDs on 
    sendCmd(c, HT1632_CMD_COMS00);   // NMOS Output 8 COM X 16 Rows X 2 Colors
  }
  
  resetBoard();
}

/******************************************************************
 * Select or de-select a particular ht1632c chip.
 * CD pins are active-low; writing 0 to the pin selects the chip.
 */
void ht1632c::chipSelect(byte chip){

  digitalWrite(_ht1632_cs, LOW);     //Writes '0' to shift register
  digitalWrite(_ht1632_clk,LOW);     //Activates first Chip
  digitalWrite(_ht1632_clk,HIGH);    
  digitalWrite(_ht1632_cs, HIGH);    //Writes '1' to shift register to disable other chips

  for (byte i = 0; i < chip; i++){   //Shift '0' to specific chip to enable it
    digitalWrite(_ht1632_clk,LOW);
    digitalWrite(_ht1632_clk,HIGH);
  }
}

void ht1632c::chipFree(){
  digitalWrite(_ht1632_cs,HIGH);      //Reset Selected Chips
  for(byte i = 0; i < 8; i++){
    digitalWrite(_ht1632_clk, LOW);
    digitalWrite(_ht1632_clk, HIGH);
  }
}

/******************************************************************
 * sendcmd
 * Send a command to the ht1632 chip. A command consists of a 3-bit "CMD" ID, an 8bit command, and one "don't care bit".
 *   Select 1 0 0 c7 c6 c5 c4 c3 c2 c1 c0 xx Free
 */
 void ht1632c::sendCmd (byte c, byte command)
{
  chipSelect(c);				   // Select chip
  writeBits(HT1632_ID_CMD, 1<<2);  // send 3 bits of id: COMMMAND
  writeBits(command, 1<<7);        // send the actual command
  writeBits(0, 1);         	       // one extra dont-care bit in commands.
  chipFree();                      //done
}

/******************************************************************
 * senddata
 * send a nibble (4 bits) of data to a particular memory location of the
 * ht1632.  The command has 3 bit ID, 7 bits of address, and 4 bits of data.
 *    Select 1 0 1 A6 A5 A4 A3 A2 A1 A0 D0 D1 D2 D3 Free
 * Note that the address is sent MSB first, while the data is sent LSB first!
 * This means that somewhere a bit reversal will have to be done to get
 * zero-based addressing of words and dots within words.
 */
 void ht1632c::sendData (byte c, byte address, byte data){
  chipSelect(c);				 // Select chip
  writeBits(HT1632_ID_WR, 1<<2); // Send ID: WRITE to RAM
  writeBits(address, 1<<6);      // Send address
  writeBits(data, 1<<3);         // Send 4 bits of data
  chipFree();                    //Done.
}

/******************************************************************
 * ht1632_writebits
 * Write bits (up to 8) to h1632 on pins ht1632_data, ht1632_wrclk Chip is assumed to already be chip-selected
 * Bits are shifted out from MSB to LSB, with the first bit sent being (bits & firstbit), shifted till firsbit is zero.
 */
void ht1632c::writeBits (byte bits, byte firstbit)
{
  while (firstbit) {
    digitalWrite(_ht1632_wrclk, LOW);
    if (bits & firstbit) {
      digitalWrite(_ht1632_data, HIGH);
    } 
    else {
      digitalWrite(_ht1632_data, LOW);
    }
    digitalWrite(_ht1632_wrclk, HIGH);
    firstbit >>= 1;
  }
}

/******************************************************************
 *point -	  light led at specific position & color
 *Syntax:     point(x,y,color)
 *Parameters: x - coordinate of SureBoard
 *            y - coordinate of SureBoard
 *            color - g for green r for red o for orange and b for black/off
 *Returns:    None
 */
void ht1632c::point(byte x , byte y, char color){

  boolean row;    //row to send data to
  byte address;   //address to send data to
  byte data;      //data to send
  

  row = y / 8;
  switch (color){
  case 'g':
	data = (addressState[x][row][0]) | (B10000000 >> (y % 8)); 
	reAddress(x, row, color, data);
	data = (addressState[x][row][1]) & (~(B10000000 >> (y % 8))); 
	reAddress(x, row, 'r', data);
	break;
  case 'r':
	data = (addressState[x][row][1]) | (B10000000 >> (y % 8)); 
	reAddress(x, row, color, data);
	data = (addressState[x][row][0]) & (~(B10000000 >> (y % 8))); 
	reAddress(x, row, 'g', data);
	break;
  case 'o':
	data = (addressState[x][row][0]) | (B10000000 >> (y % 8)); 
	reAddress(x, row, 'g', data);
	data = (addressState[x][row][1]) | (B10000000 >> (y % 8)); 
	reAddress(x, row, 'r', data);
	break;
  case 'b':
	data = (addressState[x][row][0]) & (~(B10000000 >> (y % 8))); 
	reAddress(x, row, 'g', data);
	data = (addressState[x][row][1]) & (~(B10000000 >> (y % 8))); 
	reAddress(x, row, 'r', data);
	break;
  }
 	 
}

/******************************************************************
 *reAddress - readdress entire board address  
 *Syntax:     reAddress(column, row, color, data)
 *Parameters: column - one of 32 colums of 2 rows each 
 *            row - one of 2 rows (each row consist of 8 leds)
 *            color - of the pixels
 *            data - to transefer to column 
 *Returns:    none
 */
void ht1632c::reAddress(byte column, byte row, char color, byte data){
  byte chip;
  byte addressGreen;	//for green dots on same location
  byte addressRed;		//for red dots on same location
  chip = (column > 15) + row * 2; 

  addressGreen = (column % 16) * 2;
  addressRed = 32 + (column % 16) * 2;

  switch (color){
  case 'g':
	  sendData(chip, addressGreen, data >> 4);  //send data to first half of single dot matrix 
	  sendData(chip, addressGreen + 1, data);   //send data to second half of single dot matrix
	  addressState[column][row][color == 'r'] = data;
    break;  
  case 'r':
	  sendData(chip, addressRed, data >> 4);	//send data to first half of single dot matrix 
 	  sendData(chip, addressRed + 1, data);     //send data to second half of single dot matrix              
	  addressState[column][row][color == 'r'] = data;
    break;
  }
}

/*************************************************************
 *writeRow - Copy character from chip rom memory and place it column by column. 
 *Syntax:     writeRow(message, row, color, language)
 *Parameters: message - some text to display on the board 
 *            row - board devided into 2 rows 8 leds each (0 for top row & 1 for bottom row) 
 *            color - of the text
 *            language - 'e' for english & 'h' for hebrew  
 *Returns:    none
 */
void ht1632c::writeRow(String message, boolean row, char color, char language){
  byte dots;	//dots char consist of 
  byte column = 0;
  uint8_t c;		//char to write 

	if (message.length() <= 5){
		for(char i  = 0; i < message.length(); i++){
			c  = message.charAt(i);
		
			 if ( c >= '{' && c <= '~'){
				c -= 59;
			}
			else if (c >= 97 && c <= 123 || c == 127){ //  �: � = 97 �� �: = 122 � �: = 127
					if (c == 127){
						c = 94;
					}
					else{
						c -= 29;
					}
			}
			else{
				c -= 32;
			}

			for (char n = 0; n < 5; n++){
				column += 1;
				if (language == 'e'){
					dots = pgm_read_byte_near(&Font[c][n]);
					reAddress(column , row, color, dots);
				}
				else if (language == 'h'){
					dots = pgm_read_byte_near(&Font[c][4 - n]);
					reAddress(30 - column , row, color, dots);
				}
			}
			column += 1;
		}
	}
	else if(message.length() > 5){
		message = "     " + message + "           ";
		for(byte n = 0; n < message.length() * 4 ; n++){
			for(column = 0; column < 32; column++){

				if((n + column) % 6 == 5){
					dots = B00000000;
				}
				else{
					c  = message.charAt((n + column) / 6 );

					if ( c >= '{' && c <= '~'){
						c -= 59;
					}
				else if (c >= 97 && c <= 123 || c == 127){ //  �: � = 97 �� �: = 122 � �: = 127
					if (c == 127){
						c = 94;
					}
					else{
						c -= 29;
					}
				}
				else{
					c -= 32;
				}


						if(language == 'e'){
							dots = pgm_read_byte_near(&Font[c][(n + column) % 6]);
					}
					else if(language == 'h'){
						dots = pgm_read_byte_near(&Font[c][4 - ((n + column) % 6) ]);
					}
				}

				if(language == 'e'){
					reAddress(column, row, color, dots);
				}
				else if(language == 'h'){
					reAddress(31 - column, row, color, dots);
				}
			}
		}
	}
}	
	
/******************************************************************
 *resetBoard - reset all pixel/led states 
 *Syntax:     resetBoard()
 *Parameters: none || row to reset (0 for first 1 for second row) 
 *Returns:    none
 */
void ht1632c::resetBoard(){
  for(byte i = 0; i < 32; i++){
    reAddress(i, 0, 'r', B00000000); //Reset first row 
	reAddress(i, 0, 'g', B00000000); 
    reAddress(i, 1, 'r', B00000000); //Reset second row 
	reAddress(i, 1, 'g', B00000000); 
  }
}

void ht1632c::resetBoard(boolean row){
  for(byte i = 0; i < 32; i++){
    reAddress(i, row, 'r', B00000000); //Reset some row 
	reAddress(i, row, 'g', B00000000);
  }
}
