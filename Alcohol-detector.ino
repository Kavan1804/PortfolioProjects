#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define analog_pin 0
#define motor 7
#define notDrunk 200   
#define Drunk 350 
 
float sensor_value; 

LiquidCrystal_I2C lcd(0x3F, 16, 2);
int RXPin = 5;
int TXPin = 6;
int GPSBaud = 9600;
TinyGPSPlus gps;

SoftwareSerial gpsSerial(RXPin, TXPin);
char buffer[15];

SoftwareSerial sim800a(11,10);
String lat="12.9024391";
String lon="77.5130933";
int buzzer = 9;   
//-------------------------------------------------------

void setup() {
  Serial.begin(9600); 
  gpsSerial.begin(GPSBaud);
  sim800a.begin(9600);
  delay(2000);
  sendsms("+916363599416","system online");
  Serial.println("MQ3 warming up!");
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  pinMode(motor, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(13, OUTPUT);
  lcd.print("initializing");
  delay(120000); 
  lcd.clear();
}
//----------------------------------------------------
void loop() {

  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }
   lcd.clear();
  digitalWrite(motor, HIGH);
  sensor_value = analogRead(analog_pin);
//  Serial.print("Sensor Value: ");
//  Serial.print(sensor_value);
    
  if (sensor_value < notDrunk) 
  {
    Serial.println("  |  Status: Not Drunk");
    lcd.setCursor(0,0);
    lcd.print("Data:");
    lcd.print(sensor_value);
    lcd.setCursor(0,1);
    lcd.print("Not Drunk");
    delay(1000);
    lcd.clear();
  } 
  else if (sensor_value >= notDrunk && sensor_value < Drunk) 
  {
    Serial.println("  |  Status: Drinking but within legal limits");
    lcd.setCursor(0,0);
    lcd.print("Data:");
    lcd.print(sensor_value);
    lcd.setCursor(0,1);
    lcd.print("In Limit");
    delay(1000);
    lcd.clear();
  } 
  else
  {
    displayInfo();
    digitalWrite(motor, LOW);
    sendsms("+916363599416","https://www.google.com/maps/search/rnsit+ece+block/@"+lat+","+lon);
    tone(buzzer, 1000);
    digitalWrite(13, HIGH);
    delay(500);   
    noTone(buzzer); 
    digitalWrite(13, LOW);
    delay(500); 
    tone(buzzer, 1000);
    digitalWrite(13, HIGH);
    delay(500);     
    noTone(buzzer); 
    digitalWrite(13, LOW);
    delay(500);
    Serial.println("  |  Status: DRUNK");
    lcd.setCursor(0,0);
    lcd.print("Data:");
    lcd.print(sensor_value);
    lcd.setCursor(0,1);
    lcd.print("Drunk");
    delay(1000);
    lcd.clear();
}
}
//-----------------------------------------------

void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    double lat= (gps.location.lat(), 6);
    double lon= (gps.location.lng(), 6);
    dtostrf(lat, 10, 6, buffer);
    dtostrf(lon, 10, 6, buffer);
    String la = String(buffer);
    String lo = String(buffer);
//    sendsms("+916363599416",la+lo);

  }
  else
  {
    Serial.println("Location: Not Available");
//    sendsms("+916363599416","Location not available");
  }
  
  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println("Not Available");
  }
  Serial.println();
  Serial.println();
  delay(1000);
}
void sendsms(String phoneNumber, String message) {
  sim800a.println("AT+CMGF=1"); 
  delay(1000);
  sim800a.println("AT+CMGS=\"" + phoneNumber + "\""); 
  delay(1000);
  sim800a.println(message); 
  delay(100);
  sim800a.println((char)26); 
  delay(1000);
  sim800a.println();
}
