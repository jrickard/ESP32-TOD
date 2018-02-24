/*
 Jack Rickard, jack@evtv.me.  Written February 2018
 This is an example program demonstrating the use of the Time of Day (TOD) library for the Espressif ESP32

 TOD uses the WiFi capability of ESP32 to connect to a given local access point by ssid and password using WPA.
 It sends a User Datagram Protocol UDP packet over the Internet to the NIST time server in Boulder Colorado
 and retrieves a time mark.

 Subsequently, it makes a real time clock available for time of day with the .hour(), .minute(), .second() and .millisec()
 exposed methods. This represents the actual time of day since midnight Coordinated Universal Time (UTC).  You will need
 to adjust the hour retrieved from .hour() for your local time zone.

 It does not perform any calendar/date functions - it simply provides Time of Day(TOD).

 It uses the quite accurate ESP32 hardware clock to keep the time.  It only needs to hit the NIST server once using
 the .begin(ssid,password) function to get the offset from your local machine hardware clock.  Thereafter, it 
 calculates the hour, minute, second, and millisecond internally for as long as you have power. 

 You can of course hit the NIST Time server again at any time by repeating .begin.
 
 
 
*/



#include "TOD.h"
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } //Sets up serial streaming Serial<<"text"<<variable;

TOD RTC; //Instantiate Time of Day class TOD as RTC

uint8_t lastminute=0;
uint8_t lastsecond=0;
char printbuffer[50];
bool TODvalid=false;

char ssid[] = "riverhouse";  //  your network SSID (name)
char password[] = "usatoday";       // your network password

void setup()
{ 
  Serial.begin(115200);
  
  if(RTC.begin(ssid,password))TODvalid=true;   //.begin(ssid,password) requires SSID and password of your home access point
                                               //All the action is in this one function.  It connects to access point, sends
                                               //an NTP time query, and receives the time mark. Returns TRUE on success.
  lastminute=RTC.minute();
}

void loop()
{
  if(RTC.second()!=lastsecond && TODvalid) //We want to perform this loop on the second, each second
    {
      lastsecond=RTC.second();
      sprintf(printbuffer,"   UTC Time:%02i:%02i:%02i.%03i\n", RTC.hour(), RTC.minute(),RTC.second(),RTC.millisec());
      Serial<<printbuffer; 
    }
    if(RTC.minute()==lastminute+2 && TODvalid) //Every 3 minutes, hit the NIST time server again (just to demonstrate)
    {
      lastminute=RTC.minute();
      RTC.begin(ssid,password);
    }
}


