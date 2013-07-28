// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef AClock_H_
#define AClock_H_
#include "Arduino.h"
//add your includes for the project AClock here

#include <Wire.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include "RTClib.h"
#include "ht1632c.h"
#include "MSGEQ7.h"
#include "RHT03.h"

//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project AClock here

void CheckDimPin();
void UpdateTime();
void ModeSelect();
void eqDisplay();
void getData();
//Do not add code below this line
#endif /* AClock_H_ */
