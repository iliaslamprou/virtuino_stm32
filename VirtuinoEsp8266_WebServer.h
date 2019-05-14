/* Virtuino ESP01 Library for STM32 boards ver 1.62
 * Created by Ilias Lamprou 
 * Updated Aug 06 2017
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 */


//========= VirtuinoEsp8266_WebServer Class methods  
// boolean connectESP8266_toInternet(String wifiNetworkName,String wifiNetworkPassword, int port);  Set your home wifi network SSID and PASSWORD  (Put this function on start of void setup)
// boolean createLocalESP8266_wifiServer(String wifiNetworkName,String wifiNetworkPassword, int port, int mode); Use this function to create an ESP8266 wifi local network
//                                                                    set port to 80 
//                                                                    set mode=2 to use ESP8266 only as access point 
//                                                                    set mode=3 to use ESP8266 as access point and internet station.

//  bool esp8266_setIP(byte a1, byte a2, byte a3, byte a4);           set ESP8266 local IP. Use this function after connectESP8266_toInternet function 



#ifndef VirtuinoEsp8266_WebServer_h
#define VirtuinoEsp8266_WebServer_h


#include "Arduino.h"
#include "Virtuino.h"
#ifdef USE_SOFTWARE_SERIAL
#include "SoftwareSerial.h"
#endif


#define serverInitString  "Server init..."
#define serverReadyString  "Server Ready"
#define serverNotConnectedString  "Not connected"

//====================================================================
 class VirtuinoEsp8266_WebServer : public Virtuino {
  public:                                            
  #ifdef USE_SOFTWARE_SERIAL
    VirtuinoEsp8266_WebServer(SoftwareSerial &uart);
    VirtuinoEsp8266_WebServer(SoftwareSerial &uart, uint32_t baud);
  #else 
    VirtuinoEsp8266_WebServer(HardwareSerial &uart);
    VirtuinoEsp8266_WebServer(HardwareSerial &uart, uint32_t baud);
  #endif
   virtual void run();

   void connectESP8266_toInternet(String wifiNetworkName,String wifiNetworkPassword, int port);
   void createLocalESP8266_wifiServer(String wifiNetworkName,String wifiNetworkPassword, int port, int mode=2);
   boolean esp8266_setIP(byte a1, byte a2, byte a3, byte a4);
   boolean waitForResponse(String target1,  int timeout);
   
   #ifdef USE_SOFTWARE_SERIAL
      SoftwareSerial *espSerial;
  #else
      HardwareSerial *espSerial;
  #endif  
 
  private:
   
  boolean wifiSendData(int connectionId, String* content);
  void clearESP_buffer(int waitTime);
  
 };



#endif

