#include <SoftwareSerial.h>
#include <TinyGPS++.h>
static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;
int  m = 9740;
int y = 71;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin); 
SoftwareSerial SIM800l(3, 1);
int Buzzer = 10; 
String textForSMS;
int Switch = 16; 

String datareal;
String dataimaginary;
String combined;
int raw = 1000000;

String datareal2;
String dataimaginary2;
String combined2;

double longitude;
double latitude;

void setup()
{
  SIM800l.begin(9600);
  Serial.begin(9600);
  ss.begin(GPSBaud);
  delay(10000); 
  Serial.println(" logging time completed!");
  randomSeed(analogRead(0));
  pinMode(Switch, INPUT);
  digitalWrite(Switch, HIGH);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, LOW);

  Serial.println(F("DeviceExample.ino"));
  Serial.print(F("Testing TinyGPS++ library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());

  Serial.println();
}



void sendSMS(String message)
{
  SIM800l.print("AT+CMGF=1\r");                     
  delay(100);
  SIM800l.println("AT + CMGS = \"+8801602167548\"");  
  delay(100);
  SIM800l.println(message);                         
  delay(100);
  SIM800l.println((char)26);                        
  delay(100);
  SIM800l.println();
  delay(5000);                                     

}


void makecall()
{
  SIM800l.println("ATD+ +8801602167548;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
  updateSerial();
  delay(20000); // wait for 20 seconds...
  SIM800l.println("ATH"); //hang up
  updateSerial();
}

void loop()
{
  int reading;
  
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
  
  if (digitalRead(Switch) == LOW)
  {
    makecall();
    displayInfo();
    latitude = gps.location.lat(), 6 ;
    longitude = gps.location.lng(), 6 ;
    long datareal = int(latitude);
    int shakil = ( latitude - datareal) * 100000;
    long datareal2 = int(longitude);
    int shakil2 = (longitude - datareal2 ) * 100000;
    textForSMS.concat(shakil);
    //textForSMS = "Longitude:  ";
    textForSMS.concat(datareal2);
    textForSMS = textForSMS + ".";
    textForSMS.concat(shakil2);
    //textForSMS = textForSMS + " Latitude: ";
    textForSMS.concat(datareal);
    textForSMS = textForSMS + ".";
    sendSMS(textForSMS);
    Serial.println(textForSMS);
    Serial.println("message sent.");
    delay(5000);
  }
  else
    digitalWrite(Switch, HIGH);
  digitalWrite(Buzzer, LOW);
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    SIM800l.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(SIM800l.available()) 
  {
    Serial.write(SIM800l.read());//Forward what Software Serial received to Serial Port
  }
}


void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    Serial.print(" ");
    Serial.print(F("Speed:"));
    Serial.print(gps.speed.kmph());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}