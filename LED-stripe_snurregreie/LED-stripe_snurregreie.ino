/*
Powered by:

D double
U unified
M mega
B battery

*/

#include <Bridge.h>
#include <FileIO.h>
#include <FastLED.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Process.h>

#define NUM_LEDS 5
#define perfect 66

int waterPin = A1;
int sensorPin = A0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
CRGB leds[NUM_LEDS];
Process date;
int hours;
int lastHour;

void setup(){
  Serial.begin(9600);
  while(!Serial);
  Bridge.begin();
  FileSystem.begin();
  FastLED.addLeds<NEOPIXEL, 6>(leds, NUM_LEDS);
  lcd.begin();
  
  pinMode(sensorPin, INPUT);

  if (!date.running()) {
    date.begin("date");
    date.addParameter("+%T");
    date.run();
 }
 
  String timeString = date.readString();
  Serial.println(timeString);
}

void loop(){
  Serial.println("loop");
  if (!date.running()) {
    date.begin("date");
    date.addParameter("+%T");
    date.run();
 }
  String timeString = date.readString();
  delay(100);
  Serial.println(timeString);
  int hours = timeString.substring(3, 5).toInt();
  Serial.println(hours);
  if(hours == lastHour || hours % 2 != 0){
    //return;
  }else{
    lastHour = hours;
  }
  int hydr = getHydr();
  char hydrStr[] = "";
  sprintf(hydrStr, "%d", hydr);
  //writeToFile(hydrStr, "maaling.txt", 1);
  ledStripLevel();
  printToLcd(hydr);
  pump();
}

void ledStripLevel() {
  int hydr = getHydr();
  if (hydr > perfect + 4) {  
    //⬛⬛🟦🟦🟦
    leds[0] = leds[1] = CRGB::Black;
    leds[2] = leds[3] = leds[4] = CRGB::Blue;
  } else if (hydr > perfect + 2) {  
    //⬛⬛🟦🟦⬛
    leds[0] = leds[1] = leds[4] = CRGB::Black;
    leds[2] = leds[3] = CRGB::Blue;
  } else if (hydr >= perfect) {  
    //⬛⬛🟩⬛⬛
    leds[0] = leds[1] = leds[3] = leds[4] = CRGB::Black;
    leds[2] = CRGB::Green;
  } else if (hydr > perfect - 2) {  
    //⬛🟧🟧⬛⬛
    leds[0] = leds[3] = leds[4] = CRGB::Black;
    leds[1] = leds[2] = CRGB::Orange;
  } else {  
    //🟥🟥🟥⬛⬛
    leds[3] = leds[4] = CRGB::Black;
    leds[0] = leds[1] = leds[2] = CRGB::Red;
  }
  FastLED.show();
}

void pump() {
  while(getHydr() < perfect){
    digitalWrite(waterPin, 1);
    delay(1000);
    digitalWrite(waterPin, 0);
    delay(10000);
  }
}

void writeToFile(String data, String dest, int append) { 
  //this function takes a String of data to be written, a String of desired destination file name (name only, not path or extension), and a int 1=append data 0=overwrite data.
  String destn = "/mnt/sda1/" + dest + ".txt"; //this builds the file path
  int charlen = destn.length() + 1; //this calculates the needed char length + 1 for a NULL at the end
  char destin[charlen];  //initializes the char array
  destn.toCharArray(destin, charlen); //stores the file path as a char array
  if (append == 0) FileSystem.remove(destin);  //if the option is set to overwrite the file, removes the old file
  File dataFile = FileSystem.open(destin, FILE_APPEND); //writes to the file
  if (dataFile) {  // if the file is available, write to it:
    dataFile.println(data);
    dataFile.close();
  }
}

int getHydr(){
  return 70;
  //return map(analogRead(sensorPin), 0, 1023, 0, 100);
}

void printToLcd(int h){
  lcd.clear(); 
 if(h < 40){
  lcd.print("Jeg drukner!");
 }
 else if(h < 53 && h > 47){
  lcd.print("Nok vann");
 }
 else if(h >= 53){
  lcd.print("Jeg trenger vann");
 }
 else{
  lcd.print("Error!");
 }
}
