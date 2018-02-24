# ESP32 Real Time of Day Clock for Arduino
The Arduino IDE provides an excellent library package manager where versions of libraries can be downloaded and installed.  This Github project provides the repository for the TOD support for Arduino.

The actual source of the project which is being maintained can be found here:

https://github.com/jrickard/TOD

The Time of Day or TOD library uses the WiFi capabilities of the ESP32 to 
access a local access point to gain access to the Internet.

It then sends a UDP client request to NIST.TIME.GOV and retrieves a Coordinated
Universal Time (UTC) time mark just once.

It uses that time mark to calculate an offset correction from the ESP32 hardware clock
available in the millis() function.

The UTC time since the previous midnight is available in the TOD.hour(), TOD.minute(),
TOD.second() and TOD.millisec() exposed methods.  These can be retrieved as needed
at any later time and will return a reasonable approximate Real time of day, to your
programs, precluding the need for a hardware RTC solution

No calendar or date functions are provided and you will have to perform +/- 
time zone corrections on the hour if you need local time.
