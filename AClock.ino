#include <EEPROM.h>
#include <Wire.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include "RTClib.h"
#include "I2CRom.h"
#include "ht1632c.h"
#include "RHT03.h"
#include "IR.h"


#define IRpin 3   //Digital
#define LDRPin 3  //Analog
#define red 9
#define green 10
#define blue 11

ht1632c Board1(5, 6, 7, 0); //(CS, CLK, WR, DATA, NC, NC, GND, VCC) -> (CS,CLK,WR - Port D , DATA - Port B)
RHT03 TH(4);  //Temperature & Humidity sensor
RTClib RTC;   //Real Time Clock
I2CRom Rom;
IR  IR1(IRpin);                  // IR remote
boolean statusLed[3] = {0, 1, 0}; // [R,G,B]

boolean sleep = false;
volatile boolean standBy = false;    //No movement registered in last 20 minutes

boolean DateMode = false;
boolean HumidityMode = true;
boolean TemperatureMode = true;

void setup() {
  Serial.begin(19200);
  Board1.resetBoard();
  attachInterrupt(0, goToStandBy, FALLING);  //PIR
  Board1.print("Debug", 0, 'g');
  delay(3000);
}

void loop() {

  UpdateTime();

  if (IR1.available()) receiveIR();

  adjustBrightness();

  if (Board1.scrolling) Board1.scroll(1, 100);
  else if (!standBy) ModeSelect();

  if (Serial.available() > 0) getData();

  //blink();

}

/******************************************************************
    Update Time and date on Display (if it turned on)
*/
void UpdateTime() {
  static unsigned long lastTimeUpdate;
  String time, hour, minute;
  static int column; // colon column

  //Executed each minute
  if (RTC.timeChanged()) { 
    
    RTC.getTime(hour, minute);
    time = hour + ':' + minute;
    column = Board1.print(time , 0 , 'g', ':');
    blink(column, true);
    
    // Turn on display in the morning
    if (hour == "06" && minute == "40" && standBy == true) { 
      sleep = false;
      if (!standBy) Board1.turnOn();
    }

    if (RTC.getMinute() == 0) log(); //Write to log each hour   

    lastTimeUpdate = millis();
  }
  else if (abs(millis() - lastTimeUpdate) > 62000) {
    Board1.print("??:??", 0, 'r');
    //lastTimeUpdate = millis();
  }
  else{
    blink(column, false); 
  }

   
}


/******************************************************************
    Select which data to show
*/
void ModeSelect() {
  static byte Mode;
  static unsigned long lastModeChange;

  if (abs(millis() - lastModeChange) > 3000) {
    lastModeChange = millis();

    for (int m = 0; m < 4; m++) {
      if (Mode == 0 && DateMode) {
        Board1.print(RTC.getDate(), 1, 'g');
        Mode++;
        break;
      }
      else if (Mode == 1 && HumidityMode) {
        Board1.print("H:" + (String)TH.getHumidity() + "%" , 1, 'g');
        Mode++;
        break;
      }
      else if (Mode == 2 && TemperatureMode) {
        Board1.print("T:" + (String)TH.getTemperature() + "C", 1, 'g');
        Mode++;
        break;
      }
      else {
        Mode++;
        if (Mode > 2) Mode = 0;
      }
    }
  }
}

/******************************************************************
    Get Data from Serial Port
*/

void getData() {
  String serIn;
  char character;
  char temp[3]; //for string to number convertion


  while (Serial.available()) {
    delay(2);
    character = Serial.read();
    serIn.concat(character);
  }

  //serIn = Serial.readStringUntil('\r');
  //serIn = Serial.readString();

  Serial.println(serIn);
  if (serIn.startsWith("c:")) { //c: for clock adjust - Time Format: "c: dd/mm/yy hh:mm:ss PM" <-(or AM)
    RTC.setTime(serIn);
  }
  else if (serIn.startsWith("c?")) {
    Serial.print(RTC.getDay());
    Serial.print("/");
    Serial.print(RTC.getMonth());
    Serial.print("/");
    Serial.print(RTC.getYear());
    Serial.print(" ");
    Serial.print(RTC.getHour());
    Serial.print(":");
    Serial.print(RTC.getMinute());
    Serial.print(":");
    Serial.println(RTC.getSec());
  }
  else if (serIn.startsWith("p:")) { //p for print text
    if (serIn.lastIndexOf('\r') != -1) serIn.remove(serIn.lastIndexOf('\r')); //remove new line char
    Board1.print(serIn.substring(3, serIn.length()), 1, 'g');
  }
  else if (serIn.startsWith("m?")) { //m? for mode check
    delay(100);
    Serial.println("mode: " + String(DateMode |  (HumidityMode << 1) | (TemperatureMode << 2), BIN));
  }
  else if (serIn.startsWith("m:")) { //m: for mode change
    serIn = serIn.substring(3, serIn.length());
    serIn.toCharArray(temp, serIn.length() + 1);
    DateMode = atoi(temp) & 1;
    HumidityMode = atoi(temp) & 2;
    TemperatureMode = atoi(temp) & 4;
  }
  else if (serIn.startsWith("\r\nCONNECT")) {
    statusLed[0] = 0;  //R
    statusLed[1] = 0;  //G
    statusLed[2] = 1;  //B
  }
  else if (serIn.startsWith("\r\nDISCONNECT")) {
    statusLed[0] = 0;  //R
    statusLed[1] = 1;  //G
    statusLed[2] = 0;  //B
  }
  else if (serIn.startsWith("rom?")) {  //rom? for rom read
    int progress = 0;
    Board1.print("R.O.M", 0, 'g');
    Board1.print("Read", 1, 'g');
    for (int day = 0; day < 642; day++) { //day < 642
      for (int column = 0; column < 51; column++) {
        Serial.print(Rom.read(day * 51 + column));
        Serial.print(" ");

        if ((day * 51 + column) % 512 == 0) {
          if (progress < 32) Board1.point(progress, 15, 'r');
          else Board1.point(progress - 32, 15, 'o');
          progress += 1;
        }

      }
      Serial.println();
    }
    Board1.resetBoard();
    Board1.print(RTC.getTime(), 0, 'g');
  }
}

/******************************************************************
    Write temperature & humidity values to ROM
*/
void log() {
  int address = 0x00;
  int startDay = Rom.read(0x00);
  int startMonth = Rom.read(0x01);
  int startYear = Rom.read(0x02);
  int currentHour = RTC.getHour();
  int currentDay = RTC.getDay();
  int currentMonth = RTC.getMonth();
  int currentYear = RTC.getYear();
  int current_date2days = RTC.date2days(currentDay, currentMonth, currentYear);
  int start_date2days = RTC.date2days(startDay, startMonth, startYear);

  if (startDay > 31 || startDay < 0 || startMonth > 12 || startMonth < 0 || startYear > 99 || startYear < 15 || //Junk data
      address > 32741 || current_date2days < start_date2days) {  //Rom memory exceeded or time error
    address = 0x00;
  }
  else {
    address = (RTC.date2days(currentDay, currentMonth, currentYear) - RTC.date2days(startDay, startMonth, startYear)) * 51;
  }

  if ((Rom.read(address) != currentDay) || (Rom.read(address + 1) != currentMonth) || (Rom.read(address + 2) != currentYear)) { //Clean row for new data
    Rom.write(address, currentDay);
    Rom.write(address + 1, currentMonth);
    Rom.write(address + 2, currentYear);

    for (int i = address + 3; i < 51; i++) {
      Rom.write(i, 0xff);
    }
  }

  address += currentHour * 2 + 3;

  Rom.write(address, TH.getTemperature());
  Rom.write(address + 1, TH.getHumidity());
}

/******************************************************************
     Creates 2 blinking Dots to show the activity of the clock
     column - of blinking dots
     sync - to syncronize time update with blinking
*/
void blink(int column, bool sync) {
  static unsigned long lastBlink;
  
  if (millis() - lastBlink < 500) {
    Board1.point(column, 2, 'g');
    Board1.point(column, 4, 'g');
  }
  else if (millis() - lastBlink > 1000 || sync) {
    lastBlink = millis();
  }
  else {
    Board1.point(column, 2, 'b');
    Board1.point(column, 4, 'b');
  }
}


/******************************************************************
     Adjust brightness of the board & status LED by reading ambient light sensor
*/
void  adjustBrightness() {
  static unsigned long lastCheck;
  byte PWMBoard, PWMLed;

  if (millis() - lastCheck < 1000) return;
  int LDR = analogRead(LDRPin);

  PWMBoard = map(LDR, 0, 800, 0, 15);
  Board1.setBrightness(PWMBoard);

  PWMLed = map(LDR, 0, 800, 1, 100);
  analogWrite(red, statusLed[0] * PWMLed);
  analogWrite(green, statusLed[1] * PWMLed);
  analogWrite(blue, statusLed[2] * PWMLed);

  lastCheck = millis();
}


/******************************************************************
     Go to stand by mode OR wake Up
*/

void goToStandBy() {
  standBy = true;
  attachInterrupt(0, wakeUp, RISING);
  Board1.turnOff();
}

void wakeUp() {
  standBy = false;
  attachInterrupt(0, goToStandBy, FALLING);
  if (!sleep) Board1.turnOn();
}

/******************************************************************
     Receive IR signal
*/
void receiveIR() {
  // char IRcommand;
  // IRcommand =  IR.receive();

  switch (IR1.receive()) {
    case 'P':
      static unsigned long lastPress;
      if (millis() - lastPress > 500) {
        lastPress = millis();
        if (sleep) {
          sleep = false;
          if (!standBy) Board1.turnOn();
        }
        else {
          sleep = true;
          Board1.turnOff();
        }
      }
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
      // default:
      // Serial.println("Error");   //Error
  }

}

