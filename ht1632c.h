/*
   ht1632c.h
   defintions for Holtek ht1632C LED driver.
   Updated for the HT1632C by Nick H
*/


#ifndef ht1632c_h
#define ht1632c_h

#include <Arduino.h>
/*
   commands written to the chip consist of a 3 bit "ID", followed by
   either 9 bits of "Command code" or 7 bits of address + 4 bits of data.
*/
#define HT1632_ID_CMD 4		/* ID = 100 - Commands */
#define HT1632_ID_RD  6		/* ID = 110 - Read RAM */
#define HT1632_ID_WR  5		/* ID = 101 - Write RAM */
#define HT1632_ID_BITS (1<<2)   /* IDs are 3 bits */

#define HT1632_CMD_SYSDIS 0x00	/* CMD= 0000-0000-x Turn off oscil */
#define HT1632_CMD_SYSON  0x01	/* CMD= 0000-0001-x Enable system oscil */
#define HT1632_CMD_LEDOFF 0x02	/* CMD= 0000-0010-x LED duty cycle gen off */
#define HT1632_CMD_LEDON  0x03	/* CMD= 0000-0011-x LEDs ON */
#define HT1632_CMD_BLOFF  0x08	/* CMD= 0000-1000-x Blink ON */
#define HT1632_CMD_BLON   0x09	/* CMD= 0000-1001-x Blink Off */
#define HT1632_CMD_SLVMD  0x10	/* CMD= 0001-00xx-x Slave Mode */
#define HT1632_CMD_MSTMD  0x14	/* CMD= 0001-01xx-x Master Mode */
#define HT1632_CMD_RCCLK  0x18	/* CMD= 0001-10xx-x Use on-chip clock */
#define HT1632_CMD_EXTCLK 0x1C	/* CMD= 0001-11xx-x Use external clock */
#define HT1632_CMD_COMS00 0x20	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS01 0x24	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS10 0x28	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS11 0x2C	/* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_PWM    0xA0	/* CMD= 101x-PPPP-x PWM duty cycle */
#define HT1632_CMD_BITS (1<<7)

class ht1632c {

    typedef struct {
      String message;
      byte color;
      boolean line;
      unsigned long lastMove;   // Last shift
      byte  scrollChar;         // First char to write
      byte  charColumn;         // Column of first char to write
    } QueuedMsg;


  public:
    ht1632c(byte ht1632_cs, byte ht1632_clk, byte ht1632_wrclk, byte ht1632_data);
    void point(byte x , byte y, char color);
    int print(String message, boolean line, char color, char symbol = '\0');
    void scroll(boolean row, int pause);
    boolean scrolling;
    void resetBoard();
    void resetBoard(boolean row);
    void setBrightness(byte lux);
    void turnOn();
    void turnOff();
  private:
    QueuedMsg scrollingMsg;
    int msgPixelWidth(String message);
    void chipFree();
    void chipSelect(byte chip);
    void sendCmd (byte c, byte command);
    void sendData (byte c, byte address, byte data);
    void reAddress(byte column, byte row, char color, byte data);
    void writeBits (byte bits, byte firstbit);
    byte activeChip;
    byte addressState[32][2][2];         // addressState of [column][row][color: 0 - for red, 1 - for green]
    byte _ht1632_cs;
    byte _ht1632_clk;
    byte _ht1632_wrclk;
    byte _ht1632_data;
};
#endif
