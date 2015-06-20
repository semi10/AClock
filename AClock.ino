#include <EEPROM.h>
#include <Wire.h>
#include <avr/wdt.h>
#include <avr/io.h> 
#include "RTClib.h"
#include "I2CRom.h"
#include "ht1632c.h"
#include "RHT03.h"
#include "IR.h"

#define IRpin 2
#define LDRPin 3

ht1632c Board1(4,5,6,7);  //(CS, CLK, WR, DATA, NC, NC, GND, VCC)
RHT03 TH(0);  //Temperature & Humidity sensor
RTClib RTC;   //Real Time Clock
I2CRom Rom;
IR  IR1(2); //"auxtek" IR remote

volatile boolean standby = false;
unsigned long lastTimeUpdate;

boolean DateMode = false;
boolean HumidityMode = true;
boolean TemperatureMode = true;

unsigned long startOfLoop;
int loopLength;

void setup(){
  Serial.begin(57600);
  attachInterrupt(1, sleep, RISING);
  Board1.resetBoard();
  Board1.print("Debug", 0, 'g', 1);
  Board1.load("i o i o i", 1, 'g');
  delay(4000);
}

void loop(){
  startOfLoop = millis();
  
  if(RTC.timeChanged()){ //Executed each minute
    UpdateTime(); 
    
    if(RTC.getMinute() == 0){  //Write to log each hour
      log();
    }
    
    lastTimeUpdate = millis();
  }
  else if(abs(millis() - lastTimeUpdate) > 62000){ 
    Board1.print("?? ??", 0, 'r', 0);
    lastTimeUpdate = millis();
  }
  
  if(IR1.available()) receiveIR();
  
  adjustBrightness();
 //  Board1.scroll(1, 150);
  if (!standby) ModeSelect();
  if (Serial.available() > 0) getData();
  
  blink();
  
  loopLength = millis() - startOfLoop;
  if(loopLength > 9) Serial.println(loopLength);
}

/******************************************************************
 *	Update Time and date on Display (if it turned on) 
 */

void UpdateTime(){ 
  Board1.print(RTC.getTime(), 0, 'g', 0);
  
  if (RTC.getTime() == "06 40" && standby == true){ // Turn on display in the morning
    standby = false; 
    Board1.turnOn();
  } 
}


/******************************************************************
 *	Select which data to show
 */
void ModeSelect(){
  static byte Mode; 
  static unsigned long lastModeChange;
  
  if (abs(millis() - lastModeChange) > 3000){
    lastModeChange = millis();
    
    for (int m = 0; m < 4; m++){
      if (Mode == 0 && DateMode){
        Board1.print(RTC.getDate(), 1, 'g', 0);
        Mode++;    
        break;
      }
      else if (Mode == 1 && HumidityMode){
        Board1.print("H:" + (String)TH.getHumidity() + "%" , 1, 'g', 1);
        Mode++;
        break;  
      }
      else if (Mode == 2 && TemperatureMode){
        Board1.print("T:" + (String)TH.getTemperature() + "C", 1, 'g', 1);
        Mode++;
        break;
      }
      else{
        Mode++;
        if(Mode > 2) Mode = 0;
      }
    } 
  }
}

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
  if (serIn.compareTo("R") == 0){
    wdt_enable(WDTO_15MS); //Tells watchdog to reset after 15ms for remote sketch upload
  }
  else if (serIn.startsWith("setTime:")){ //setTime: for clock adjust
    RTC.setTime(serIn);
  }
  else if (serIn.startsWith("print:")){ //print: for write text
    Board1.resetBoard(0);
    Board1.print(serIn.substring(6, serIn.length()), 0, 'g', 0);
    if (serIn.length() <= 11) delay(1500);
    Board1.resetBoard(0);
    Board1.print(RTC.getTime(), 0, 'g', 0);
  }
  else if (serIn.startsWith("mode?")){  //mode? for mode check
    delay(100);
    Serial.println("mode: " + String(DateMode |  (HumidityMode << 1) | (TemperatureMode << 2), BIN));
  }
  else if (serIn.startsWith("mode:")){ //mode: for mode change
    serIn = serIn.substring(3, serIn.length());
    serIn.toCharArray(temp, serIn.length() + 1);
    DateMode = atoi(temp) & 1;
    HumidityMode = atoi(temp) & 2;
    TemperatureMode = atoi(temp) & 4;
  }
  else if(serIn.startsWith("rom?")){    //rom? for rom read 
     Board1.print("R.O.M", 0, 'g', 1);
     Board1.print("Read...", 1, 'g', 1);
     for(int day = 0; day < 642; day++){
      for(int column = 0; column < 51; column++){
        Serial.print(Rom.read(day * 51 + column));
        Serial.print(" ");
      }
      Serial.println();
    }
    Board1.resetBoard();
    Board1.print(RTC.getTime(), 0, 'g', 0);
  }
}

/******************************************************************
 *	Put display into sleep mode
 */
void sleep(){
  static unsigned long lastPress;
   
  if (millis() - lastPress > 500){
    lastPress = millis();
    if(standby){
      standby = false;
      Board1.turnOn();
    }
    else{
      standby = true;
      Board1.turnOff();
    }
  }
}


/******************************************************************
 *	Write temperature & humidity values to ROM
 */
 void log(){
   unsigned int address = 0x00; 
   int startDay = Rom.read(0x00);  
   int startMonth = Rom.read(0x01);
   int startYear = Rom.read(0x02);
   int currentHour = RTC.getHour();
   int currentDay = RTC.getDay();
   int currentMonth = RTC.getMonth();
   int currentYear = RTC.getYear();
   
   if(startDay > 31 || startDay < 0 || startMonth > 12 || startMonth < 0 || startYear > 99 || startYear < 15){
     Rom.write(0x00, currentDay);
     Rom.write(0x01, currentMonth);
     Rom.write(0x02, currentYear);     
     
     for(int i = 3; i < 51; i ++){
       Rom.write(i, 0xff);
     }
     
     address = 0x03;
   } 
   else{
     address = (RTC.date2days(currentDay, currentMonth, currentYear) - RTC.date2days(startDay, startMonth, startYear)) * 51;
     
     if(address > 32741) address = 0x00; //Rom memory exceeded
     
     if(Rom.read(address) != currentDay || (Rom.read(address + 1) != currentMonth) || (Rom.read(address + 2) != currentYear)){  //Clean row for new data
       Rom.write(address, currentDay);
       Rom.write(address + 1, currentMonth);
       Rom.write(address + 2, currentYear); 
       
       for(int i = address + 3; i < 51; i ++){
         Rom.write(address, 0xff);
       } 
       
     }
     address += currentHour * 2 + 3; 
     Serial.print("Address to print to: ");
     Serial.println(address); 
   }
   
     Rom.write(address, TH.getTemperature());
     Rom.write(address + 1, TH.getHumidity()); 
 }

/******************************************************************
 *	 Creates 2 blinking Dots to show the activity of the clock
 */
void blink(){
  static unsigned long lastBlink;
  
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


/******************************************************************
 *	 Adjust brightness of the board by reading ambient light sensor
 */
void  adjustBrightness(){
  static unsigned long lastCheck;
  byte PWMBoard;
  int LDR = analogRead(LDRPin);
  
  if(millis() - lastCheck < 1000) return;
  
  PWMBoard = map(LDR, 0, 1024, 0, 15);   
//  Serial.print(LDR);
//  Serial.print("    ");
//  Serial.println(PWMBoard);
  Board1.setBrightness(PWMBoard);
  lastCheck = millis();
}


/******************************************************************
 *	 Receive IR signal
 */
void receiveIR(){
// char IRcommand;
 // IRcommand =  IR.receive();
  
    switch(IR1.receive()){
    case 'P':
      sleep();
      break;  
    case 'L':
      Serial.println("L");    //Louder
      break;
    case 'Q':
      Serial.println("Q");     //Quiet
      break;
    case '+':
      Serial.println("+");
      break;
    case '-':
      Serial.println("-");
      break;
    default:
      Serial.println("Error");   //Error
  }
  
}

