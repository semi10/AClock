/* MSGEQ7.h
 * defintions for MSGEQ7 Graphic Equalizer 
 */

#ifndef MSGEQ7_h
#define MSGEQ7_h

#include <Arduino.h>

  	  struct Band{
	  short amplitude[7];		//stores previos states of band 
	  short peak;			
	  char peakColor;
	  short difference;
	};

class MSGEQ7{
  public:
	  MSGEQ7(byte _resetPin, byte _strobePin ,byte _eqPin);
	  void resetEQ();
	  char getColor(short value);
	  short getpAmplitude(byte band)	{return freq[band].amplitude[1];}
	  short getDifference(byte band)	{return freq[band].difference;}
	  short getPeak(byte band)			{return freq[band].peak;}
	  char getPeakColor(byte band)		{return freq[band].peakColor;}
	  void eqRead(byte);
	  boolean signalAvailable(); 
  private:
	Band freq[7];
	unsigned long lastAudioSignal;
	byte resetPin;
	byte strobePin;
    byte eqPin;
	};
#endif

