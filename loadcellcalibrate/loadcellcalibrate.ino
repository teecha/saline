#include "HX711.h"
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set these to run example.
#define FIREBASE_HOST "saline123.firebaseio.com"
#define FIREBASE_AUTH "ONKX2NjGtFSdCirtkizJxbsRJTnRcPQHXca38llL"
#define WIFI_SSID "S6 edge"
#define WIFI_PASSWORD "snist456"
LiquidCrystal_I2C lcd(0x27, 16, 2);
float weight;
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D6;
const int LOADCELL_SCK_PIN = D7;
Servo myservo;// Name the servo
HX711 scale;// Initilaize scale
void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  myservo.attach(D10);
  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC
  scale.set_scale(-848.5);          // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                    // reset the scale to 0
  Serial.println("After setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());// print a raw reading from the ADC
  Serial.println("Readings:");
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  //float initialweight = scale.get_units();
  float weight = scale.get_units();
  Firebase.setFloat("initialweight", weight);
}
//float initialweight = scale.get_units();
void loop()
{
  int i;
  float weight = scale.get_units();
  Serial.println(weight);
  Serial.print("one reading:\t");
  Serial.println(scale.get_units(), 1);
  Firebase.setFloat("weight", weight);
  Firebase.pushFloat("weightlog", weight);
  Firebase.setFloat("serialval", i++);
  // Serial.println("upload successful");
  // handle error
  if (Firebase.failed()) {
    Serial.print("weight upload failed:");
    Serial.println(Firebase.error());
  }
  //  Firebase.setFloat("percent", percent);
  float gotweight = Firebase.getFloat("weight");
  float gweight = Firebase.getFloat("initialweight");
  Serial.println(gotweight);
  int percent = ((gweight - weight) / gweight) * 100;
  Serial.println(percent);
  lcd.setCursor(1, 0);
  lcd.print("percent");
  lcd.setCursor(2, 1);
  lcd.print(percent);
  /*if(gotweight <= 20)
    {
    for(int pos = 0;pos<=90;pos++)
    {
      myservo.write(pos);
      Firebase.setString("Warning","bottle running out");
      delay(10);
      if(gotweight > 20){exit;}
     }
    }
    if(gotweight >= 500)
    {
    myservo.write(10);
    }*/

}
