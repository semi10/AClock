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
#define led1 8
#define led2 9
#define led3 10
#define led4 11

ht1632c::ht1632c(byte ht1632_cs, byte ht1632_clk, byte ht1632_wrclk, byte ht1632_data) {
  _ht1632_cs = ht1632_cs;		//Chip select signal input
  _ht1632_clk = ht1632_clk;		//Chip select clock signal input
  _ht1632_wrclk = ht1632_wrclk;	        //Write data clock input
  _ht1632_data = ht1632_data;		//Data input

  DDRD |= (1 << _ht1632_cs);       //ht1632_cs as output  
  DDRD |= (1 << _ht1632_clk);      //_ht1632_clk as output
  DDRD |= (1 << _ht1632_wrclk);    //_ht1632_wrclk as output
  DDRB |= (1 << _ht1632_data);     //_ht1632_data as output
  //pinMode(_ht1632_cs, OUTPUT);
 // pinMode(_ht1632_clk, OUTPUT);
 // pinMode(_ht1632_wrclk, OUTPUT);
 // pinMode(_ht1632_data, OUTPUT);
  
  
  for (int chip = 0; chip < 4; chip++) {
    sendCmd(chip, HT1632_CMD_SYSON);    // System on
    sendCmd(chip, HT1632_CMD_LEDON);    // LEDs on
    sendCmd(chip, HT1632_CMD_COMS00);   // NMOS Output 8 COM X 16 Rows X 2 Colors
    sendCmd(chip, HT1632_CMD_PWM+5);
    sendCmd(chip, HT1632_CMD_SYSDIS); 
    sendCmd(chip, HT1632_CMD_SYSON); 
  }

  resetBoard();
}

/******************************************************************
 * Select or de-select a particular ht1632c chip.
 * CD pins are active-low; writing 0 to the pin selects the chip.
 */
void ht1632c::chipSelect(byte chip){   
  PORTD &= ~(B00000001 << _ht1632_cs);   //cs-LOW:   Writes '0' to shift register
  PORTD &= ~(B00000001 << _ht1632_clk);  //clk-LOW:  Activates first Chip
  PORTD |= B00000001 << _ht1632_clk;     //clk-HIGH
  PORTD |= B00000001 << _ht1632_cs;      //cd-HIGH:  Writes '1' to shift register to disable other chips 

  for (activeChip = 0 ; activeChip < chip; activeChip++){   //Shift '0' to specific chip to enable it
    PORTD &= ~(B00000001 << _ht1632_clk); //clk-LOW
    PORTD |= B00000001 << _ht1632_clk;    //clk-HIGH
  }
}

void ht1632c::chipFree(){
  PORTD |= B00000001 << _ht1632_cs; //cs-HIGH: Reset Selected Chips
    
  for(byte i = 0; i < 4; i++){      //4 for 1 board only (8 for 2)
    PORTD &= ~(B00000001 << _ht1632_clk); //clk-LOW
    PORTD |= B00000001 << _ht1632_clk;    //clk-HIGH
  }
  
}

/******************************************************************
 * sendcmd
 * Send a command to the ht1632 chip. A command consists of a 3-bit "CMD" ID, an 8bit command, and one "don't care bit".
 *   Select 1 0 0 c7 c6 c5 c4 c3 c2 c1 c0 xx Free
 */
void ht1632c::sendCmd (byte chip, byte command)
{
  chipSelect(chip); // Select chip
  writeBits(HT1632_ID_CMD, 1 << 2);         // send 3 bits of id: COMMMAND
  writeBits(command, 1 << 7);               // send the actual command
  writeBits(0, 1);         	            // one extra dont-care bit in commands.
  chipFree();
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
void ht1632c::sendData (byte chip, byte address, byte data) {
  chipSelect(chip); // Select chip
  writeBits(HT1632_ID_WR, 1 << 2);           // Send ID: WRITE to RAM
  writeBits(address, 1 << 6);                // Send address
  writeBits(data, 1 << 3);                   // Send 4 bits of data
  chipFree();
}

/******************************************************************
 * ht1632_writebits
 * Write bits (up to 8) to h1632 on pins ht1632_data, ht1632_wrclk Chip is assumed to already be chip-selected
 * Bits are shifted out from MSB to LSB, with the first bit sent being (bits & firstbit), shifted till firsbit is zero.
 */
void ht1632c::writeBits (byte bits, byte firstbit)
{
  while (firstbit) {
    PORTD &= ~(B00000001 << _ht1632_wrclk);    //wrclk-LOW

    if (bits & firstbit) {
      PORTB |= B00000001 << _ht1632_data;      //data-HIGH
    }
    else {
      PORTB &= ~(B00000001 << _ht1632_data);  //data-LOW
    }
    PORTD |= B00000001 << _ht1632_wrclk;    //wrclk-HIGH
    firstbit >>= 1;
  }
}

/******************************************************************
 *point -light led at specific position & color
 *Syntax:     point(x,y,color)
 *Parameters: x - coordinate of SureBoard
 *            y - coordinate of SureBoard
 *            color - g for green r for red o for orange and b for black/off
 *Returns:    None
 */
void ht1632c::point(byte x , byte y, char color) {
  
  byte chip;
  byte addressGreen;	//for green dots on same location
  byte addressRed;      //for red dots on same location

  
  boolean row;    //row to send data to
  byte data;      //data to send

  row = y / 8;
  chip = (x > 15) + row * 2;
  addressGreen = (x % 16) * 2;
  addressRed = 32 + (x % 16) * 2;


  
  switch (color) {
    case 'g':
      if(addressState[x][row][1] != (addressState[x][row][1] | (B10000000 >> (y % 8)))){
        data = (addressState[x][row][1] | (B10000000 >> (y % 8)));
        sendData(chip, addressGreen, data >> 4);  //send data to first half of single dot matrix
        sendData(chip, addressGreen + 1, data);   //send data to second half of single dot matrix
        addressState[x][row][1] = data;
      }
      if(addressState[x][row][0] != (addressState[x][row][0] & ~(B10000000 >> (y % 8)))){
        data = (addressState[x][row][0] & ~(B10000000 >> (y % 8)));
        sendData(chip, addressRed, data >> 4);    //clear first half of single dot matrix
        sendData(chip, addressRed + 1, data);     //clear second half of single dot matrix
        addressState[x][row][0] = data; 
      }
      break;
    case 'r':
      if(addressState[x][row][0] != (addressState[x][row][0] | (B10000000 >> (y % 8)))){
        data = (addressState[x][row][0]) | (B10000000 >> (y % 8));
        sendData(chip, addressRed, data >> 4);    //send data to first half of single dot matrix
        sendData(chip, addressRed + 1, data);     //send data to second half of single dot matrix
        addressState[x][row][0] = data; 
      }
      if(addressState[x][row][1] != (addressState[x][row][1] & ~(B10000000 >> (y % 8)))){
        data = (addressState[x][row][1] & ~(B10000000 >> (y % 8)));
        sendData(chip, addressGreen, data >> 4);  //clear first half of single dot matrix
        sendData(chip, addressGreen + 1, data);   //clear second half of single dot matrix
        addressState[x][row][1] = data;
      }
      break;
    case 'o':
      if(addressState[x][row][1] != (addressState[x][row][1] | (B10000000 >> (y % 8)))){
        data = (addressState[x][row][1] | (B10000000 >> (y % 8)));
        sendData(chip, addressGreen, data >> 4);  //send data to first half of single dot matrix
        sendData(chip, addressGreen + 1, data);   //send data to second half of single dot matrix
        addressState[x][row][1] = data;
      }
      if(addressState[x][row][0] != (addressState[x][row][0] | (B10000000 >> (y % 8)))){
        data = (addressState[x][row][0]) | (B10000000 >> (y % 8));
        sendData(chip, addressRed, data >> 4);    //send data to first half of single dot matrix
        sendData(chip, addressRed + 1, data);     //send data to second half of single dot matrix
        addressState[x][row][0] = data; 
      }
      break;
    case 'b':
      if(addressState[x][row][0] != (addressState[x][row][0] & ~(B10000000 >> (y % 8)))){
        data = (addressState[x][row][0] & ~(B10000000 >> (y % 8)));
        sendData(chip, addressRed, data >> 4);    //clear first half of single dot matrix
        sendData(chip, addressRed + 1, data);     //clear second half of single dot matrix
        addressState[x][row][0] = data; 
      }
      if(addressState[x][row][1] != (addressState[x][row][1] & ~(B10000000 >> (y % 8)))){
        data = (addressState[x][row][1] & ~(B10000000 >> (y % 8)));
        sendData(chip, addressGreen, data >> 4);  //clear first half of single dot matrix
        sendData(chip, addressGreen + 1, data);   //clear second half of single dot matrix
        addressState[x][row][1] = data;
      }
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
void ht1632c::reAddress(byte column, byte row, char color, byte data) {
  byte chip;
  byte addressGreen;	//for green dots on same location
  byte addressRed;      //for red dots on same location
  chip = (column > 15) + row * 2;

  addressGreen = (column % 16) * 2;
  addressRed = 32 + (column % 16) * 2;

  switch (color) {
    case 'g':
      if(addressState[column][row][1] != data){
        sendData(chip, addressGreen, data >> 4);  //send data to first half of single dot matrix
        sendData(chip, addressGreen + 1, data);   //send data to second half of single dot matrix
        addressState[column][row][1] = data;
      }
      if(addressState[column][row][0] != 0x00){
        sendData(chip, addressRed, 0x00);         //send data to first half of single dot matrix
        sendData(chip, addressRed + 1, 0x00);     //send data to second half of single dot matrix
        addressState[column][row][0] = 0x00; 
      }
      break;
    case 'r':
      if(addressState[column][row][0] != data){
        sendData(chip, addressRed, data >> 4);    //send data to first half of single dot matrix
        sendData(chip, addressRed + 1, data);     //send data to second half of single dot matrix
        addressState[column][row][0] = data;        
      }
      if(addressState[column][row][1] != 0x00){
        sendData(chip, addressGreen, 0x00);         //send data to first half of single dot matrix
        sendData(chip, addressGreen + 1, 0x00);     //send data to second half of single dot matrix
        addressState[column][row][1] = 0x00; 
      }
      break;
    case 'o':
      if(addressState[column][row][1] != data){
        sendData(chip, addressGreen, data >> 4);  //send data to first half of single dot matrix
        sendData(chip, addressGreen + 1, data);   //send data to second half of single dot matrix
        addressState[column][row][1] = data;
      }
      if(addressState[column][row][0] != data){
        sendData(chip, addressRed, data >> 4);    //send data to first half of single dot matrix
        sendData(chip, addressRed + 1, data);     //send data to second half of single dot matrix
        addressState[column][row][0] = data;
      } 
      break;
  }
}

/*************************************************************
 *writeRow - Copy character from chip rom memory and place it column by column.
 *Syntax:     writeRow(message, row, color)
 *Parameters: message - some text to display on the board
 *            row - board devided into 2 rows 8 leds each ('0' for top row & '1' for bottom row)
 *            color - of the text
 *            noGap - Leave no gaps inside characters ('0' for gaps & '1' for no gaps)
 *Returns:    none
 */
void ht1632c::print(String message, boolean line, char color, boolean noGap) {
  row[line].oldMsg.txt = message + "      ";
  row[line].oldMsg.color = color;
  
  byte dots;	                // Dots char consist of
  char currentChar;             // Char to write
  byte charColumn = 0;          // Char array consists of 5 columns         
  byte boardColumn;         // Board consists of 32 columns 
  
  if(noGap) boardColumn = 0;
  else{
    reAddress(0 , line, 'o', 0x00);
    boardColumn = 1;
  }
  
  for (byte charNumber = 0; charNumber < message.length(); charNumber++) {
    currentChar = message.charAt(charNumber) - 32;
    
    for (byte charColumn = 0; charColumn < 6; charColumn++) {
      if(boardColumn == 32) return;                      // Length of the board exceeded
      dots = pgm_read_byte_near(&Font[currentChar][charColumn]);
      if(dots == 0x00  && currentChar != 0 && charColumn != 5 && noGap) continue;    // Ignor spaces between chars (but not space char itself " ")
      reAddress(boardColumn , line, color, dots);
      boardColumn += 1;
   } 
 }
  for(boardColumn; boardColumn < 32; boardColumn++){    //Write till the end of the board to wipe previous data
    reAddress(boardColumn , line, 'o', 0x00);
  }
}

/*************************************************************
 *load - Loads text to the board.
 *Syntax:     load(message, row, color)
 *Parameters: message - some text to display on the board
 *            row - board devided into 2 rows 8 leds each (0 for top row & 1 for bottom row)
 *            color - of the text
 *Returns:    none
 */
void ht1632c::load(String message, boolean line, char color){
  if(row[line].oldMsg.txt == ""){
    row[line].oldMsg.txt =  "      " + message + "      ";
    row[line].oldMsg.color = color; 
  }
  else{
    row[line].newMsg.txt =  "      " + message + "      ";
    row[line].newMsg.color = color; 
  }
}


/*************************************************************
 *scroll - scrolls characters on board from old row to newer.
 *Syntax:     scrollRow(pause)
 *Parameters: pause - delay between rotations 
 *Returns:    boolean - '0' scroll in progress, '1' end of scroll
 */
 boolean ht1632c::scroll(boolean line, int pause){
   byte dots;	                 // Dots char consist of
   char currentChar;             // Char to write
   byte charColumn = 0;          // Char array consists of 5 columns         
   byte boardColumn = 0;         // Board consists of 32 columns 
   char scrollChar = row[line].scrollChar;   // Char to start scroll from
   String txt = row[line].oldMsg.txt;        // Text to write on board
   
   
   if((millis() - row[line].lastMove) > pause){
     row[line].lastMove = millis();
      
     for (byte charNumber = scrollChar; charNumber < txt.length(); charNumber++) {    //Print the chars
       currentChar = txt.charAt(charNumber) - 32;
    
       if(charNumber == scrollChar){
         if(row[line].charColumn != 5 && currentChar != 0){ //Ignore gaps besides spaces between two chars         
           while(pgm_read_byte_near(&Font[currentChar][row[line].charColumn]) == 0x00){
             row[line].charColumn++;      //Skip spaces at the beginning of the char 
           }
         }     
         charColumn = row[line].charColumn;
       }
       else{
         charColumn = 0;
       }
         
       for (charColumn; charColumn < 6; charColumn++) {    //Prints the columns of the char
      
         if(boardColumn > 31) {  // Length of the board exceeded
           row[line].scrollChar += (row[line].charColumn + 1) / 6;
           row[line].charColumn = (row[line].charColumn + 1) % 6;
           return 0;  //There is more data to scroll                    
         }
         
         dots = pgm_read_byte_near(&Font[currentChar][charColumn]);
         if(dots == 0x00  && currentChar != 0 && charColumn != 5) continue;    // Ignor spaces between chars (but not space char itself " ")         

         reAddress(boardColumn , line, row[line].oldMsg.color, dots);

         boardColumn += 1;
       }         
    }
    
    if(row[line].newMsg.txt != ""){  //If there is new text to scroll
      row[line].oldMsg.txt = row[line].newMsg.txt;
      row[line].oldMsg.color = row[line].newMsg.color; 
      row[line].newMsg.txt = "";
    }
    row[line].scrollChar = 0;
    row[line].charColumn = 0;
    return 1; //Scroll finish
  }
  return 0;
}

/******************************************************************
 *resetBoard - reset all pixel/led states
 *Syntax:     resetBoard()
 *Parameters: none || row to reset (0 for first 1 for second row)
 *Returns:    none
 */
void ht1632c::resetBoard() {  
  for (byte chip = 0; chip < 4; chip++) {
    for(byte address = 0; address < 64; address++){
      sendData(chip, address, 0x00);
    }
  }
    for(byte column = 0; column < 32; column++){
      addressState[column][0][0] = 0x00;
      addressState[column][0][1] = 0x00;
      addressState[column][1][0] = 0x00;
      addressState[column][1][1] = 0x00;
  }
}

void ht1632c::resetBoard(boolean row) {
  if(row = 0){
    for (byte chip = 0; chip < 2; chip++) {
      for(byte address = 0; address < 64; address++){
        sendData(chip, address, 0x00);
      }
    }
  }
  else{
    for (byte chip = 2; chip < 4; chip++) {
      for(byte address = 0; address < 64; address++){
        sendData(chip, address, 0x00);
      }
    }
  }
  for(byte column = 0; column < 32; column++){
    addressState[column][row][0] = 0x00;
    addressState[column][row][1] = 0x00;
  }
}

/******************************************************************
 *setBrightness - Change PWM of the board
 *Syntax:     setBrightness(lux)
 *Parameters: lux - From 0 to 15 (0 = 1/16 duty, 16 = 16/16 duty)
 *Returns:    none
 */
void ht1632c::setBrightness(byte lux) {
  for (byte c = 0; c < 4; c++) {
    sendCmd(c, HT1632_CMD_PWM + lux);
  }
}

/******************************************************************
 *turnOn - Turn on the display
 *Syntax:     turnOn
 *Parameters: none
 *Returns:    none
 */
void ht1632c::turnOn(){
  for(int chip = 0; chip < 4; chip++){
    sendCmd(chip, HT1632_CMD_SYSON);  
    sendCmd(chip, HT1632_CMD_LEDON);
  } 
}

/******************************************************************
 *turnOff - Turn off the display
 *Syntax:     turnOff
 *Parameters: none
 *Returns:    none
 */
void ht1632c::turnOff(){
    for(int chip = 0; chip < 4; chip++){
      sendCmd(chip, HT1632_CMD_LEDOFF);
      sendCmd(chip, HT1632_CMD_SYSDIS);  
  } 
}
