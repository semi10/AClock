/* ProgressBar.h
 *
 */
#ifndef ProgressBar_h
#define ProgressBar_h
#include <avr/pgmspace.h> 
#include <Arduino.h>
#include "ht1632c.h"

class ProgressBar{
public:
  ProgressBar(ht1632c *Board1);
  void animateProgress();
  void set(int newVal);
  int getVal(){return currentVal;}  
  void redraw();
  void clear();
  void setInProgress(bool state){inProgress = state;}
  bool getInProgress(){return inProgress;}
private:
  ht1632c *Board1;
  int currentVal;
  int currentAnimationVal;    //Place of green dot
  unsigned long lastTimeAnimated;
  bool inProgress;            //True if ProgressBar animation needed
};

#endif
