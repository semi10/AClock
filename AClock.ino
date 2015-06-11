#include <Wire.h>
#include <avr/wdt.h>
#include <avr/io.h> 
#include "RTClib.h"
#include "ht1632c.h"
//#include "MSGEQ7.h"
//#include "RHT03.h"

//#define dimPin 8

//MSGEQ7 EQ1(2,3,0);
ht1632c Board1(4,5,6,7);  //(CS, CLK, WR, DATA, NC, NC, GND, VCC)
//RHT03 TH1(5); //Temperature & Humidity sensor
RTClib RTC;

//uint8_t Mode = 0; 
unsigned long lastTimeCheck;
//unsigned long lastModeChange;
//boolean EqualizerWorking = false;
boolean DimDisplay = false;

//boolean DateMode = true;
//boolean EqualizerMode = false;
//boolean HumidityMode = true;
//boolean TemperatureMode = true;

void setup(){
  Board1.resetBoard();
  Board1.print("Debug!", 1, 'g', 1);
  delay(2000);
  
  Serial.begin(57600);
//  pinMode(dimPin, INPUT);
  Serial.println(B10000001);
}

void loop(){


//  CheckDimPin();
  UpdateTime();
//  if (!DimDisplay) ModeSelect();
  if (Serial.available() > 0) getData();
}

/******************************************************************
 *	Update Time and date on Display (if it turned on) 
 */

void UpdateTime(){
  static unsigned long lastBlink;
 
  if (!DimDisplay){
    if (RTC.timeChanged()) {
      Board1.print(RTC.getTime(), 0, 'g', 0);
      lastTimeCheck = millis();
    }
    else if (abs(millis() - lastTimeCheck) > 62000) {
      Board1.print("??:??", 0, 'g', 0);
      // Board1.print("??/??", 1, 'g', 'e');
      lastTimeCheck = millis();
    } 
    // Creates 2 blinking Dots to show the activity of the clock
    if(millis() - lastBlink < 500){
      Board1.point(15, 2, 'g');
      Board1.point(15, 4, 'g');
    }
    else if(millis() - lastBlink > 1000){
      lastBlink = millis();
    }
    else{ 
      Board1.point(15, 2, 'b');
      Board1.point(15, 4, 'b');
    }
  }
}

/******************************************************************
 *	Check if dim button presed 
 */
 /*
void CheckDimPin(){
  if (digitalRead(dimPin)){
    delay(200); //debounce

    Serial.println(DimDisplay,BIN);

    //Turn Display On/Off
    if (DimDisplay){  
      DimDisplay = false;
      Board1.print(RTC.getTime(), 0, 'g', 'e');
    }
    else{

      DimDisplay = true;
    }
    Board1.resetBoard();
  }

  //Turn on Display at specified time
  if (RTC.getTime() == "05:50" && DimDisplay == true) DimDisplay = false;

}
*/
/******************************************************************
 *	Select which data to show
 */
 /*
void ModeSelect(){
  if (EqualizerMode & EQ1.signalAvailable()){
    if (!EqualizerWorking){
      EqualizerWorking = true;
      Board1.resetBoard(1);
      EQ1.resetEQ();
    }
    eqDisplay(); 
  }
  else if (abs(millis() - lastModeChange) > 3000){
    lastModeChange = millis();
    EqualizerWorking = false;
    Board1.resetBoard(1);

    for (int m = 0; m < 4; m++){
      if (Mode == 0 && DateMode){
        Board1.print(RTC.getDate(), 1, 'g', 'e');
        Mode++;    
        break;
      }
      else if (Mode == 1 && HumidityMode){
        Board1.print(TH1.getHumidity() , 1, 'g', 'e');
        Mode++;
        break;  
      }
      else if (Mode == 2 && TemperatureMode){
        Board1.print(TH1.getTemperature(), 1, 'g', 'e');
        Mode++;
        break;
      }
      else {
        Mode ++;
        if (Mode > 2) Mode = 0;
      }
    } 
  }
}
*/
/******************************************************************
 *	Show equalizer on a board
 */
 /*
void eqDisplay(){
  for (byte y = 0; y < 7 ;y++){
    short difference = EQ1.getDifference(y); //Difference between current equalizer state an previous equalizer state
    short pAmplitude =  EQ1.getpAmplitude(y);
    short peak = EQ1.getPeak(y);

    if (peak < 31) Board1.point(peak + 1, y + 9, 'b');

    if (difference > 0){  
      for (byte z = 0; z <= difference; z++){
        Board1.point(pAmplitude + z, y + 9, EQ1.getColor(pAmplitude + z));
      }
    }
    else if (difference < 0){
      for (int z = 0; z > difference; z--){
        Board1.point(pAmplitude + z, y + 9, 'b');
      }
    } 
    Board1.point(peak, y + 9, EQ1.getPeakColor(y)); 
  }
}
*/
/******************************************************************
 *	Get Data from Serial Port
 */
 
void getData(){
  String serIn;
  char character;
  char temp[3]; //for string to number convertion

  while (Serial.available()){
    delay(20);
    character = Serial.read();
    serIn.concat(character);
  }
  Serial.println(serIn);
  if (serIn.compareTo( "R") == 0){
    wdt_enable(WDTO_15MS); //Tells watchdog to reset after 15ms for remote sketch upload
  }
  else if (serIn.startsWith("c:")){ //c: for clock adjust
    RTC.setTime(serIn);
  }
  else if (serIn.startsWith("t:")){ //t: for write text
    Board1.resetBoard(0);
    Board1.print(serIn.substring(6, serIn.length()), 0, 'g', 0);
    if (serIn.length() <= 11) delay(1500);
    Board1.resetBoard(0);
    Board1.print(RTC.getTime(), 0, 'g', 'e');
  }
 /* else if (serIn.startsWith("m?")){  //m? for mode check
    delay(100);
    Serial.println("m: " + String(DateMode | (EqualizerMode << 1) | (HumidityMode << 2) | (TemperatureMode << 3), BIN));
  }
  else if (serIn.startsWith("m:")){ //m: for mode change

    serIn = serIn.substring(3, serIn.length());
    serIn.toCharArray(temp, serIn.length() + 1);

    DateMode = atoi(temp) & 1;
    EqualizerMode = atoi(temp) & 2;
    HumidityMode = atoi(temp) & 4;
    TemperatureMode = atoi(temp) & 8;

  }*/
}



