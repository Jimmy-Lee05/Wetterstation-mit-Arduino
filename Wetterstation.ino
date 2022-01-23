#include <OneWire.h>
#include <DallasTemperature.h> //Install Manager: DS18B20 DallasTemperature
#include <virtuabotixRTC.h> //https://github.com/chrisfryer78/ArduinoRTClibrary
#include <LiquidCrystal_I2C.h>//https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <Adafruit_BME280.h> //Install Manager: bme280
#include <Time.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2); //I2C address 0x3F or 0x27 - 16 characters in 2 lines
OneWire oneWire(2);// Pin 2 temp sensors
DallasTemperature sensors(&oneWire);
virtuabotixRTC myRTC(6, 7, 8);

Adafruit_BME280 bme;
int tempSensorAmount = 3;
boolean mode = true; // to change the LCD preview

void setup(void) {
  Serial.begin(9600);

  pinMode(SS, OUTPUT);

  sensors.begin();
  sensors.setResolution(12);

  if (!bme.begin(0x76, &Wire)) { //I2C address 0x76 or 0x77
    Serial.println(F("BME280 sensor not found, wrong address?"));
    while (true);
  }

  lcd.begin(); //LCD starting...
  
  lcd.setCursor(0,0);
  Serial.print(F("Init SD card..."));
  if (!SD.begin(4)) {// SD Shield Pin 4 in use
    Serial.println(F("Card failed, or not present"));
    lcd.print(F("Er"));
    while(1) {};
  }
  Serial.println(F("SD initialized."));
  lcd.setCursor(0,0);
  lcd.print("0K");
  Serial.println(F("SD Card OK."));
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {//if data exists:
      dataFile.println(F("Time,Temp1,Temp2,Temp3,WaterLevel,Temp4,Humidity,Pressure"));
      dataFile.close();
  }
  //seconds, minutes, hours, day of the week, day of the month, month, year
  //smyRTC.setDS1302Time(0, 56, 17, 3, 19, 1, 2022);
}

void loop(void) {
  String dataString = "";

  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("                ");
  
  sensors.requestTemperatures(); // load temperature
  for(int x=0;x < tempSensorAmount; x++) {
    float temp = sensors.getTempCByIndex(x);
    Serial.println("Temp " + String(x+1) + ": " + temp);
    if(mode){
      lcd.setCursor(0+x*5,1);
      lcd.print(temp,1);
    }
    dataString += String(temp) + ",";
  }

  int waterLevel = analogRead(3);
  Serial.println("Water-Level: " + String(waterLevel));
  lcd.setCursor(13,0);
  lcd.print(waterLevel);

  float temperature = bme.readTemperature();
  float hum = bme.readHumidity();
  float pressure = bme.readPressure()/100;  // Pa --> hPa 
  Serial.println("Humidity: " + String(hum)+ " Temperature: " + String(temperature) + " Pressure: " + String(pressure));
  lcd.setCursor(3,0);
  lcd.print(String(round(hum))+"%");
  if(!mode){
    lcd.setCursor(0,1);
    lcd.print(String(temperature)+ " " + pressure + "hPa");
  }
  
  lcd.setCursor(7,0);
  myRTC.updateTime();
  lcd.print(String(myRTC.hours) + ":" + String(myRTC.minutes));
  Serial.print(String(myRTC.hours) + ":" + String(myRTC.minutes));
  WriteText(Time() + "," + dataString + waterLevel + "," + temperature + "," + hum + "," + pressure);
  Serial.println(" ");
  mode=!mode;
  delay(30000);
}

void WriteText(String txt){
  File myFile = SD.open("datalog.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(txt);
    myFile.close();
  } else {Serial.println(F("error opening datalog.txt"));}//error can't open datalog.txt
}

String Time(){
  myRTC.updateTime();
  return String(myRTC.dayofmonth) + "/" + String(myRTC.month) + "/" + String(myRTC.year) + " " + String(myRTC.hours) + ":" + String(myRTC.minutes) + ":" + String(myRTC.seconds);
}
