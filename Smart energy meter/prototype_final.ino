#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>                                           // firebase library
#include<Wire.h>
#include <LiquidCrystal_I2C_Hangul.h>
#include "EmonLib.h"
#include <TimeLib.h>

EnergyMonitor emon;
LiquidCrystal_I2C_Hangul lcd(0x27, 16, 2);

#define FIREBASE_HOST "smart-energy-meter-e48aa-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "VEZSWYxOsbChZu8B7mkpghm2prMYvEESoUNz6LCv"
#define WIFI_SSID "RSES"                                           // input your home or public wifi name 
#define WIFI_PASSWORD "46yur1fvfa"

#define vCalibration 150 //106.8
#define currCalibration 1.3
const int buzzer=2;
const int waterflow = 25;
const int gasflow = 4;
int sum_tot = 0;
int sum_ele = 0;
int sum_wat = 0;
int sum_gas = 0;
const int curr_min = minute();
volatile long pulseone;
volatile long pulsetwo;
unsigned long lastTimeone;
unsigned long lastTimetwo;
float volume;
const int mq135 = 32;
float sensorValue;
const int relay = 14;
int VRMS = 0;
float IRMS = 0;
float TPOWER = 0;
float TKWH = 0;
int i = 0;
float kWh = 0;
unsigned long lastmillis = millis();

float calculate_electricity_cost(float electricity_per_hour) {
    // Average price in Indian rupees (INR) per kWh
    float electricity_price_per_unit = 7.5;  // INR per kWh
    // Calculate total cost of electricity per hour
    float electricity_cost_per_hour = round(electricity_per_hour * electricity_price_per_unit * 100) / 100; // Round to 2 decimal places
    return electricity_cost_per_hour;
}

float calculate_water_cost(float water_per_hour) {
    // Average price in Indian rupees (INR) per litre
    float water_price_per_litre = 0.52;  // INR per litre
    // Calculate total cost of water per hour
    float water_cost_per_hour = round(water_per_hour * water_price_per_litre * 100) / 100; // Round to 2 decimal places
    return water_cost_per_hour;
}

float calculate_gas_cost(float gas_flow_rate_per_hour) {
    // Average price in Indian rupees (INR) per meter cube
    float gas_price_per_meter_cube = 1.96;  // INR per meter cube
    // Calculate total cost of gas per hour
    float gas_cost_per_hour = round(gas_flow_rate_per_hour * gas_price_per_meter_cube * 100) / 100; // Round to 2 decimal places
    return gas_cost_per_hour;
}

float calculate_total_bill(float electricity_cost_per_hour, float water_cost_per_hour, float gas_cost_per_hour) 
{
    // Calculate total bill per hour
    float total_bill_per_hour = round(electricity_cost_per_hour + water_cost_per_hour + gas_cost_per_hour * 100) / 100; // Round to 2 decimal places
    float total_bill = total_bill_per_hour + (0.18 * total_bill_per_hour); // GST
    return total_bill;
}

void myTimerEvent() {
  emon.calcVI(20, 2000);
  Serial.print("Vrms: ");
  Serial.print(emon.Vrms, 2);
  Serial.print("V");

  Serial.print("\tIrms: ");
  Serial.print(emon.Irms, 4);
  Serial.print("A");

  Serial.print("\tPower: ");
  Serial.print(emon.apparentPower, 4);
  Serial.print("W");

  Serial.print("\tkWh: ");
  kWh = kWh + emon.apparentPower * (millis() - lastmillis) / 3600000000.0;
  Serial.print(kWh, 4);
  Serial.println("kWh");
   lastmillis = millis();

  if ( emon.Vrms > 300)
  {
    VRMS = 0;
    IRMS = 0;
    TPOWER = 0;
  }

  else if (emon.Vrms < 200 )
  {
    VRMS = 0;
    IRMS = 0;
    TPOWER = 0;
  }

  else
  {
    VRMS = emon.Vrms;
    IRMS = emon.Irms;
    TPOWER = emon.apparentPower;
    TKWH = kWh;
  }
  
  lcd.clear();
  Firebase.setFloat("VRMS", emon.Vrms);
  lcd.setCursor(0, 0);
  lcd.print("V:");
  delay(50);
  lcd.setCursor(2, 0);
  lcd.print( emon.Vrms);
  delay(300);
  Firebase.setFloat("IRMS", emon.Irms);
  lcd.setCursor(6, 0);
  lcd.print("I:");
  delay(50);
  lcd.setCursor(8, 0);
  lcd.print(emon.Irms);
  delay(300);
  Firebase.setFloat("TPOWER", emon.apparentPower);
  lcd.setCursor(0, 1);
  lcd.print("W: ");
  delay(50);
  lcd.setCursor(2, 1);
  lcd.print(emon.apparentPower);
  delay(300);
  Firebase.setFloat("TKWH", kWh);
  lcd.setCursor(6, 1);
  lcd.print("KW: ");
  delay(50);
  lcd.setCursor(8, 1);
  lcd.print(kWh);
  delay(300);
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("WELCOME");
  delay(200);
  digitalWrite(relay, HIGH);
  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  emon.current(34, currCalibration); // Current: input pin, calibration.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                      //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                                      //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                       // connect to firebase
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("WIFI CONNECTED");
  delay(200);
  pinMode(buzzer, OUTPUT);
  pinMode(waterflow, INPUT);
  pinMode(gasflow, INPUT);
  delay(200);
  attachInterrupt(digitalPinToInterrupt(waterflow), increaseWater, RISING);
  attachInterrupt(digitalPinToInterrupt(gasflow), increaseGas, RISING);
  pinMode(relay, OUTPUT);
}


void readmq() {
  sensorValue = analogRead(mq135);
   Serial.print(sensorValue);
  if (sensorValue > 500)
  {
    Serial.println(" GAS Detected ");
    Firebase.setString("mq135", "Gas detected");
    digitalWrite(buzzer, HIGH);
    Serial.println(sensorValue);
  }
  else
  {
    Serial.println(" No GAS Detected");
    Firebase.setString("mq135", " NO Gas detected");
    digitalWrite(buzzer, LOW);
  }
  delay(1000); // wait 2s for next reading
}

void readwater() {
  float volume1 = 2.663 * pulseone / 1000 * 30;
  if (millis() - lastTimeone > 2000) {
    pulseone = 0;
    lastTimeone = millis();
  }
  Serial.print(volume1);
  Serial.println(" L/day");
  Firebase.setFloat("waterflow(litre per minute)", volume1);
}

void readgas() {
  volume = 2.663 * pulsetwo / 1000 * 30;
  if (millis() - lastTimetwo > 2000) {
    pulsetwo = 0;
    lastTimetwo = millis();
  }
  Serial.print(volume);
  Serial.println(" mtrcube/h");
  Firebase.setFloat("Gasflow", volume);
}

void increaseWater() {
  pulseone++;
}

void increaseGas() {
  pulsetwo++;
}

void loop() {  
    emon.calcVI(20, 2000);
    float electricity_per_hour = emon.apparentPower / 1000;  // Convert to kWh
    float water_per_hour = 2.663 * pulseone / 1000 * 30;  // Convert to litres per hour
    float gas_flow_rate_per_hour = 2.663 * pulsetwo / 1000 * 30;  // Convert to meter cube per minute per hour

    float electricity_cost_per_hour = calculate_electricity_cost(electricity_per_hour);
    float water_cost_per_hour = calculate_water_cost(water_per_hour);
    float gas_cost_per_hour = calculate_gas_cost(gas_flow_rate_per_hour);
    float total_bill_per_hour = calculate_total_bill(electricity_cost_per_hour, water_cost_per_hour, gas_cost_per_hour);
    sum_tot += total_bill_per_hour;
    sum_ele += electricity_cost_per_hour;
    sum_wat += water_cost_per_hour;
    sum_gas += gas_cost_per_hour;    
    
    myTimerEvent();
    readmq();
    readwater();
    readgas();
    Serial.println("Total bill is:  ");
    Serial.print(sum_tot);
    Serial.println("Electricity bill is:  ");
    Serial.print(sum_ele);
    Serial.println("Water bill is:  ");
    Serial.print(sum_wat);
    Serial.println("Gas bill is:  ");
    Serial.print(sum_gas);
    Firebase.setFloat("Total_Bill_Per_hour", sum_tot);
    Firebase.setFloat("Electricity_Bill_Per_hour", sum_ele);
    Firebase.setFloat("Water_Bill_Per_hour", sum_wat);
    Firebase.setFloat("Gas_Bill_Per_hour", sum_gas);


}