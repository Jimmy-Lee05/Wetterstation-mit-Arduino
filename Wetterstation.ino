#include <OneWire.h>
#include <DallasTemperature.h> //Install Manager: DS18B20 DallasTemperature
#include <virtuabotixRTC.h> //https://github.com/chrisfryer78/ArduinoRTClibrary
#include <LiquidCrystal_I2C.h>//https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <Time.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);   // 0der (0x27)  Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall einer mit 16 Zeichen in 2 Zeilen.
OneWire oneWire(2);// Pin 2 für Temp Sensoren
DallasTemperature sensors(&oneWire);
virtuabotixRTC myRTC(6, 7, 8);

int tempSensorAmount = 2;

void setup(void) {
  Serial.begin(9600);

  sensors.begin();//Starte die Sensoren
  sensors.setResolution(12);
  
  pinMode(SS, OUTPUT);

  lcd.begin(); //Startet das LCD Display
  lcd.setCursor(0,0);
  lcd.print(F("Init SD card"));
  Serial.print(F("Init SD card..."));

  if (!SD.begin(4)) {// Pin 4 wird genutzt
    Serial.println(F("Card failed, or not present"));
    lcd.print(F("SD Error"));
    while(1) {};
  }
  Serial.println(F("SD initialized."));
  lcd.setCursor(0,0);
  lcd.print(F("SD OK;          "));
  Serial.println("SD Card OK.");
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {//Wenn Datein vorhanden:
      dataFile.println(F("Time,Temp1,Temp2,WaterLevel"));
      dataFile.close();
  }
  //seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(1, 26, 23, 5, 12, 11, 2021);
}

void loop(void) {
  int waterLevel = 0;
  String dataString = "";
  float temp[tempSensorAmount];
  waterLevel = analogRead(3);
  
  Serial.print("Water-Level: ");
  Serial.println(waterLevel);
  lcd.setCursor(13,0);
  lcd.print(waterLevel);
  
  sensors.requestTemperatures(); // Laden der Temperaturen

  for(int x=0;x < tempSensorAmount; x++) {
    temp[x]=sensors.getTempCByIndex(x);
    Serial.println("Temp " + String(x) + ": " + temp[x]);
    lcd.setCursor(0+x*5,1);
    lcd.print(temp[x],1);
    dataString += String(temp[x]) + ",";
  }
  lcd.setCursor(7,0);
  myRTC.updateTime();
  lcd.print(String(myRTC.hours) + ":" + String(myRTC.minutes));
  WriteText(Time() + "," + dataString + waterLevel);
  delay(30000);
}

void WriteText(String txt){
  File myFile = SD.open("datalog.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(txt);
    myFile.close();
  } else {Serial.println(F("error opening datalog.txt.txt"));}// Fehler wenn die Datei nicht geöffnet werden kann
}

String Time(){
  myRTC.updateTime();
  return String(myRTC.dayofmonth) + "/" + String(myRTC.month) + "/" + String(myRTC.year) + " " + String(myRTC.hours) + ":" + String(myRTC.minutes) + ":" + String(myRTC.seconds);
}
