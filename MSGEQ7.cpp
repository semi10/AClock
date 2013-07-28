/******************************************************************
 *Writen for MSGEQ7 - Graphic Equalizer Display Filter 
 */
#include "MSGEQ7.h"

MSGEQ7::MSGEQ7(byte _resetPin, byte _strobePin, byte _eqPin){
	resetPin = _resetPin;
	strobePin = _strobePin;
	eqPin = _eqPin;

	DDRD = DDRD | (1 << resetPin) | (1 << strobePin);		//resetPin & strobePin = outputs
	PORTD = PORTD | (1 << resetPin);						//resetPin = hight
	delayMicroseconds(100);
	PORTD = PORTD & ~(1 << resetPin);						//resetPin = low
	delayMicroseconds(100);
}

/******************************************************************
 *	Read current equalizer values
 */
void MSGEQ7::eqRead(byte _band){
	byte band;
	band = _band;

	if (band == 6){									//start from first band
		PORTD = PORTD | (1 << resetPin);			//resetPin = hight
		delayMicroseconds(50);
		PORTD = PORTD & ~(1 << resetPin);			//resetPin = low
		delayMicroseconds(25);
	}
	else{											//go to next band
		PORTD = PORTD | (1 << strobePin);			//strobePin = hight
		delayMicroseconds(50);
		PORTD = PORTD & ~(1 << strobePin);			//strobePin = low
		delayMicroseconds(25);
	}
	
	for (byte i = 6; i > 0; i--){	
		freq[band].amplitude[i] = freq[band].amplitude[i-1];
	}


	freq[band].amplitude[0] = analogRead(0) / 32;
	freq[band].difference = freq[band].amplitude[0] - freq[band].amplitude[1];

	if (freq[band].amplitude[0] >= freq[band].peak){
		freq[band].peak = freq[band].amplitude[0];
		freq[band].peakColor = getColor(freq[band].peak);
	}
	else{
		freq[band].peak -= 1;
	}
	
}

/******************************************************************
 *	Determine appropreate color for dot
 */

  char MSGEQ7::getColor(short value){
		if (value < 16){
			return 'g';
		}
		else if (value >= 24){
			return 'r';
		}
		else{
			return 'o';
		}
  }

/******************************************************************
 *	Check for audio source
 */
boolean MSGEQ7::signalAvailable(){
	int average = 0; 
	int amp = 0;

	for (byte b = 0; b < 7; b++){
		 eqRead(b);
		for (byte a = 0; a < 7; a++){
			amp = freq[b].amplitude[a];
		average += amp;
		}
	}

	average = average / 49;

	if (average > 5){ 
		lastAudioSignal = millis();
		return true;
	}
	else if (abs(millis() - lastAudioSignal) < 15000){
		return true;
	}
	else{
		return false;
	}
}

/******************************************************************
 *	Reset Equalizer State
 */
void MSGEQ7::resetEQ(){
	for (byte b = 0; b < 7; b++){
		freq[b].amplitude[0] = 0;
	}
}