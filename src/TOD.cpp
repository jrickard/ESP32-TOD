#include "TOD.h"

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 


TOD::TOD()  // Define the constructor.
  
{

}


TOD::~TOD() //Define destructor
{
  }

  bool TOD::begin(char *ssid,char *password)
  { 
    bool success=false;
    if(getAccessPoint(ssid,password))
      {
        UDP.begin(2390);//Local port to listen for NTP response packets is 2390
        if(getNTPTime())
          {
            Serial<<"Success...we have a NIST time mark...";
            char printbuffer[100];
            sprintf(printbuffer,"NIST-F1 UTC Time:%02i:%02i:%02i.%03i (or thereabouts...)\n\n", hour(), minute(),second(),millisec());
            Serial<<printbuffer; 
            
            success=true;
          }
        
      }
    return success;
  }
  
bool TOD::getAccessPoint(char *ssid,char *password)
{
  bool success=false;
  WiFi.begin(ssid,password);
  long timestamp=millis();
  Serial<<"\nSearching for WiFi SSID:"<<ssid<<"....";

  while ( WiFi.status()!= WL_CONNECTED && millis()-timestamp<10000)
    {
    } //Wait up to 10 seconds for connection
   
  if(WiFi.status()==WL_CONNECTED)
    {
      Serial<<"\nConnected to WiFi SSID:"<<WiFi.SSID()<<" IP Address:"<<WiFi.localIP()<<" Signal Strength:"<<WiFi.RSSI()<<"dBm\n";
      success=true;
    }
    else Serial<<"Sorry....access point unavailable at this time..";
    return success;
}

bool TOD::getNTPTime()
{
  //All of this really only produces two outputs into global variables: 
  //NTPmillis provides us with the number of UTC milliseconds TODAY, discarding all from 1900 to last midnight
  //NTPtimeStamp is the exact number of local machine millis WHEN the UPD packet was received.
  //This is what we need to use local machine clocks to derive UTC real time of day
  
  bool success=false;
  NTPtimeStamp=0;
  int8_t i=0;
  while(NTPtimeStamp==0 and i++ < 3)  //We'll try sending a packet and waiting for a response three times.
    {
      sendNTPpacket(); 
      long looptime=millis();
      while(millis()-looptime<15000)  //We'll wait up to 15 second for a response
        {
          if (UDP.parsePacket()) 
            {
              NTPtimeStamp=millis(); //Time stamp when NTP packet was first received             
              UDP.read(packetBuffer, 48); // read the packet into the buffer

              //First 32-bit integer contains number of whole seconds since 00:00 1 Jan 1900
              uint32_t secondsSince1900 = word(packetBuffer[40], packetBuffer[41])<<16; //Load high word and shift left 16 bits
              secondsSince1900 |=  word(packetBuffer[42], packetBuffer[43]);            //OR result with low word to complete

              NTPmillis=(secondsSince1900 % 86400)*1000;  //Number of whole seconds since midnight x 1000 for milliseconds
                             
              //Second 32-bit integer contains fractions of a second to potentially 230 picosecond granularity
              uint32_t fracsec = word(packetBuffer[44], packetBuffer[45])<<16; //Load high word and shift left 16 bits
              fracsec |=  word(packetBuffer[46], packetBuffer[47]);            //OR result with low word to complete               
              uint32_t NTPmillisecond=(double)fracsec*1000/0xFFFFFFFF;         

              NTPmillis+=NTPmillisecond; //Totalize all milliseconds since last midnight UTC
              if(NTPmillis>0)success=true;
              break;
            }   
        }
    }
  return success;
}

void TOD::sendNTPpacket()
{ 
  memset(packetBuffer, 0, 48); //Clear buffer
  
  // Create UDP packet to form NTP request
  // https://tools.ietf.org/html/rfc5905 describes RFC5905 Network Time Protocol
 
  packetBuffer[0]  = 0b11100011;   // Leap Indicator(3), Version(4), Mode is client (3)
  packetBuffer[1]  = 0;            // Stratum, or type of clock. 0 is top level.
  packetBuffer[2]  = 6;            // Poll Interval.  Typically 6 min 10 max
  packetBuffer[3]  = 0xEC;         // Precision Exponent - signed integer.  -18 = 1 microsecond
  packetBuffer[12] = 0x49;         // I   kiss code
  packetBuffer[13] = 0x4E;         // N   Not yet synchronized for first time
  packetBuffer[14] = 0x49;         // I
  packetBuffer[15] = 0x54;         // T

  IPAddress NISTtimeServer(129, 6, 15, 28); // format IP address for time.nist.gov NTP server 
  UDP.beginPacket(NISTtimeServer, 123);     //NTP requests are to port 123
  UDP.write(packetBuffer, 48);              //Send our packet buffer
  UDP.endPacket();
}
long TOD::rtc()
{  //Returns the number of milliseconds in the hour minute and second of the UDP packet PLUS the time that has 
   //elapsed in milliseconds since we received the packet
   
      if(millis()<NTPtimeStamp)getNTPTime(); //Millis overflow condition

  long Elapsed=millis() - NTPtimeStamp; //Current clock time - NTP packet received time 
  long RTCmillis=NTPmillis+Elapsed;  //NTPhour+minute+second in millis PLUS time since UDP packet

      if(RTCmillis<millis())getNTPTime(); //RTCmillis overflow condition

   return RTCmillis;
}

uint8_t TOD::hour()
{
  uint8_t hour = rtc()/3600000;
  return hour;
}

uint8_t TOD::minute()
{
  uint8_t minute = (rtc() % 3600000) / 60000;
  return minute;
}

uint8_t TOD::second()
{
   uint8_t second = (rtc() % 60000) / 1000;
  return second;
}

int16_t TOD::millisec()
{
   uint16_t millisecond = rtc() % 1000;
  return millisecond;
}



