/* Virtuino ESP01 Library for STM32 boards ver 1.62
 * Created by Ilias Lamprou
 * Updated Aug 06 2017
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 */


#ifndef VirtuinoEthernet_WebServer_h
#define VirtuinoEthernet_WebServer_h


#include "Arduino.h"
#include "Virtuino.h"

 #if defined  STM32PACKAGE
   #include <Ethernet_STM.h>
#else
  #include <Ethernet.h>
#endif


#define clientConnected  "Connected"
#define clientDisconnected  "Disconnected"

 class VirtuinoEthernet_WebServer: public Virtuino  {
  public:
    VirtuinoEthernet_WebServer(EthernetServer* server);
    void run();
    private:
    EthernetServer* ethernetServer;
 };

#endif

