// copied from http://heim.ifi.uio.no/haakoh/avr/a
#ifndef NewFont_h
#define NewFont_h

#include <avr/pgmspace.h>                // Enable data to be stored in Flash Mem as well as SRAM  

unsigned const char PROGMEM Font[96][5] = {
/*00*/ {0, 0, 0, 0, 0}, // space
/*01*/ {0x00, 0x00, 0x7D, 0x00, 0x00}, // !
/*02*/ {0x00, 0x10, 0x20, 0x10, 0x20}, // "
/*03*/ {0x0A, 0x1F, 0x0A, 0x1F, 0x0A}, // #
/*04*/ {0x31, 0x49, 0x7F, 0x49, 0x46}, // &
/*05*/ {0x33, 0x36, 0x0C, 0x1B, 0x33}, // %
/*06*/ {0x2E, 0x51, 0x5B, 0x24, 0x0A}, // &
/*07*/ {0x00, 0x00, 0x70, 0x00, 0x00}, // '
/*08*/ {0x00, 0x1C, 0x63, 0x00, 0x00}, // (
/*09*/ {0x00, 0x00, 0x63, 0x1C, 0x00}, // )
/*10*/ {0x22, 0x14, 0x3E, 0x14, 0x22}, // *
/*11*/ {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
/*12*/ {0x00, 0x01, 0x02, 0x00, 0x00}, // ,
/*13*/ {0x08, 0x08, 0x08, 0x08, 0x08}, // - 
/*14*/ {0x01, 0x00, 0x00, 0x00, 0x00}, // .
/*15*/ {0x01, 0x06, 0x08, 0x30, 0x40}, // /
  
/*16*/ {0x3E, 0x45, 0x49, 0x51, 0x3E}, // 0 
/*17*/ {0x00, 0x21, 0x7F, 0x01 ,0x00}, // 1
/*18*/ {0x21, 0x43, 0x45, 0x49, 0x31}, // 2
/*19*/ {0x22, 0x41, 0x49, 0x49, 0x36}, // 3 
/*20*/ {0x0C, 0x14, 0x24, 0x7F, 0x04}, // 4
/*21*/ {0x72, 0x51, 0x51, 0x51, 0x4E}, // 5
/*22*/ {0x3E, 0x49, 0x49, 0x49, 0x26}, // 6
/*23*/ {0x40, 0x40, 0x4F, 0x50, 0x60}, // 7
/*24*/ {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
/*25*/ {0x32, 0x49, 0x49, 0x49, 0x3E}, // 9
  
/*26*/ {0x00, 0x00, 0x14, 0x00, 0x00}, // :
/*27*/ {0x00, 0x01, 0x12, 0x00, 0x00}, // ;
/*28*/ {0x08, 0x14, 0x14, 0x22, 0x22}, // <
/*29*/ {0x14, 0x14, 0x14, 0x14, 0x14}, // =
/*30*/ {0x22, 0x22, 0x14, 0x14, 0x08}, // >
/*31*/ {0x00, 0x30, 0x45, 0x48, 0x30}, // ?
/*32*/ {0x1E, 0x2D, 0x55, 0x5D, 0x3A}, // @
  
/*33*/ {0x3f, 0x48, 0x48, 0x48, 0x3f}, // A 
/*34*/ {0x7f, 0x49, 0x49, 0x49, 0x36}, // B 
/*35*/ {0x3e, 0x41, 0x41, 0x41, 0x22}, // C
/*36*/ {0x7f, 0x41, 0x41, 0x22, 0x1c}, // D
/*37*/ {0x7f, 0x49, 0x49, 0x49, 0x41}, // E
/*38*/ {0x7f, 0x48, 0x48, 0x48, 0x40}, // F
/*39*/ {0x3e, 0x41, 0x49, 0x49, 0x2e}, // G
/*40*/ {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H
/*41*/ {0x00, 0x41, 0x7f, 0x41, 0x00}, // I
/*42*/ {0x06, 0x01, 0x01, 0x01, 0x7e}, // J
/*43*/ {0x7f, 0x08, 0x14, 0x22, 0x41}, // K
/*44*/ {0x7f, 0x01, 0x01, 0x01, 0x01}, // L
/*45*/ {0x7f, 0x20, 0x10, 0x20, 0x7f}, // M
/*46*/ {0x7f, 0x10, 0x08, 0x04, 0x7f}, // N
/*47*/ {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O
/*48*/ {0x7f, 0x48, 0x48, 0x48, 0x30}, // P
/*49*/ {0x3e, 0x41, 0x45, 0x42, 0x3D}, // Q
/*50*/ {0x7f, 0x48, 0x4c, 0x4a, 0x31}, // R
/*51*/ {0x31, 0x49, 0x49, 0x49, 0x46}, // S
/*52*/ {0x40, 0x40, 0x7f, 0x40, 0x40}, // T
/*53*/ {0x7e, 0x01, 0x01, 0x01, 0x7e}, // U
/*54*/ {0x7c, 0x02, 0x01, 0x02, 0x7c}, // V
/*55*/ {0x7f, 0x02, 0x04, 0x02, 0x7f}, // W
/*56*/ {0x63, 0x14, 0x08, 0x14, 0x63}, // X
/*57*/ {0x60, 0x10, 0x0f, 0x10, 0x60}, // Y
/*58*/ {0x43, 0x45, 0x49, 0x51, 0x61}, // Z
  
/*59*/ {0x00, 0x3E, 0x41, 0x41, 0x00}, // [
/*60*/ {0x40, 0x30, 0x08, 0x06, 0x01}, // slash
/*61*/ {0x00, 0x40, 0x20, 0x00, 0x00}, // `
/*62*/ {0x00, 0x41, 0x41, 0x3E, 0x00}, // ]
/*63*/ {0x10, 0x20, 0x40, 0x20, 0x10}, // ^
/*64*/ {0x01, 0x01, 0x01, 0x01, 0x01}, // _
/*65*/ {0x00, 0x08, 0x36, 0x41, 0x00}, // {
/*66*/ {0x00, 0x00, 0x7F, 0x00, 0x00}, // |
/*67*/ {0x00, 0x41, 0x36, 0x08, 0x00}, // }
/*68*/ {0x08, 0x10, 0x08, 0x04, 0x08}, // ~ 

/*69*/ {0xDC, 0x60, 0x30, 0x18, 0xEC}, // א 
/*70*/ {0x84, 0x84, 0x84, 0X7C, 0x04}, // ב
/*71*/ {0x00, 0x8C, 0x90, 0xA0, 0x7C}, // ג
/*72*/ {0x80, 0x80, 0x80, 0xFC, 0x80}, // ד
/*73*/ {0x9C, 0x80, 0x80, 0x80, 0x7C}, // ה
/*74*/ {0x00, 0x00, 0xFC, 0x00, 0x00}, // ו
/*75*/ {0x00, 0x80, 0xFC, 0x40, 0x00}, // ז
/*76*/ {0xFC, 0x80, 0x80, 0x80, 0x7C}, // ח
/*77*/ {0xFC, 0x04, 0x84, 0x84, 0x78}, // ט 
/*78*/ {0x00, 0x40, 0xC0, 0x40, 0x00}, // י
/*79*/ {0x80, 0x80, 0x80, 0x80, 0x7F}, // ך
/*80*/ {0x84, 0x84, 0x84, 0x84, 0x7C}, // כ
/*81*/ {0x00, 0xC4, 0x48, 0x50, 0x20}, // ל
/*82*/ {0xFC, 0x84, 0x84, 0x84, 0x7C}, // ם
/*83*/ {0xBC, 0x40, 0x84, 0x84, 0x7C}, // מ
/*84*/ {0x00, 0x80, 0xFF, 0x00, 0x00}, // ן
/*85*/ {0x04, 0x04, 0x04, 0x04, 0xFC}, // נ
/*86*/ {0xF8, 0x84, 0x84, 0x84, 0x78}, // ס
/*87*/ {0x04, 0xFC, 0x04, 0x04, 0xF8}, // ע
/*88*/ {0xE0, 0xA0, 0x80, 0x7F, 0x00}, // ף
/*89*/ {0xEC, 0xA4, 0x84, 0x84, 0x7C}, // פ
/*90*/ {0x80, 0x60, 0x1F, 0x20, 0xC0}, // ץ
/*91*/ {0x84, 0x44, 0x24, 0x14, 0xE8}, // צ
/*92*/ {0xBF, 0x80, 0x88, 0x90, 0x60}, // ק
/*93*/ {0x80, 0x80, 0x80, 0x80, 0x7C}, // ר
/*94*/ {0xFC, 0x04, 0xFC, 0x04 ,0xF8}, // ש 
/*95*/ {0x04, 0xFC, 0x80, 0x80, 0x7C}, // ת
   
};
#endif