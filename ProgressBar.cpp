/* ProgressBar.h

*/
#include "ProgressBar.h"

ProgressBar::ProgressBar(ht1632c *Board1): Board1(Board1){  //Constructor
  currentVal = 0;
  currentAnimationVal = 0;
  lastTimeAnimated = millis();
  inProgress = false;
}

void ProgressBar::set(int newVal) {
  if (newVal > currentVal) {   //If need to print new dots
    for (int i = currentVal + 1; i <= newVal; i++) {
      Board1->point(i - 1, 7, 'o');
    }
  }
  else {                        //If need to clear dots
    for (int i = newVal + 1; i <= 32; i++) {
      Board1->point(i - 1, 7, 'b');
    }
  }
  currentVal = newVal;         //Update current value
}


void ProgressBar::animateProgress(){
  if (abs(millis() - lastTimeAnimated) < 150) return;
  lastTimeAnimated = millis();

  if (currentVal <= 1) return;
  
  Board1->point(currentAnimationVal, 7, 'o');
  currentAnimationVal =  (currentAnimationVal + 1) % currentVal;
  Board1->point(currentAnimationVal, 7, 'g');
}

void ProgressBar::redraw(){
  for (int i = 1; i <= currentVal; i++){
    Board1->point(i - 1, 7, 'o');
  }
}

void ProgressBar::clear(){
  currentVal = 0;
  currentAnimationVal = 0;
  for (int i = 1; i <= 32; i++){
    Board1->point(i - 1, 7, 'b');
  }
}

