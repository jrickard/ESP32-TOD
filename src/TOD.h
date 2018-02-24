#ifndef TOD_h
#define TOD_h
/*
Jack Rickard (jack@evtv.me) February 2018
Real Time clock in software for Espressif ESP32 chip.
Provides time of day through .hour(), .minute(), .second(), and .millisec()
exposed methods.

*/

#include <WiFi.h>
#include <WiFiUdp.h>

class TOD 
{
  public:
        TOD();
       ~TOD();
        bool begin(char*,char*);
        bool getAccessPoint(char*,char*);
        void sendNTPpacket();
        bool getNTPTime();
        uint8_t hour();
        uint8_t minute();
        uint8_t second();
        int16_t millisec();

  private:
      WiFiUDP UDP;
      long NTPtimeStamp=0;
      long NTPmillis=0;
      byte packetBuffer[48];
      protected:
      long rtc();
 

};
#endif /* TOD_h */

