/* Virtuino ESP01 Library for STM32 boards ver 1.62
 * Created by Ilias Lamprou
 * Updated Aug 06 2017
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 */



#include "VirtuinoEthernet_WebServer.h"
#include <SPI.h>


#if defined  STM32PACKAGE
  #include <Ethernet_STM.h>
#else
  #include <Ethernet.h>
#endif

 //========================================================================================= VirtuinoEthernet_WebServer
 //=========================================================================================
  
 VirtuinoEthernet_WebServer::VirtuinoEthernet_WebServer(EthernetServer* server):Virtuino::Virtuino(){
  ethernetServer=server;
}

 
 
 //========================================================================================= run
 //=========================================================================================
 
 void VirtuinoEthernet_WebServer::run(){
  EthernetClient client = ethernetServer->available();
  if (client) {
    char c;
   if (DEBUG) Serial.println(clientConnected);
    while (client.connected()) {
      if (client.available()) {
        c = client.read();
        commandBuffer+=c;
        if (DEBUG)  Serial.write(c);
      }
      if (c == '\n' ) {
              int pos = commandBuffer.indexOf("GET /");
              if (pos!=-1){                                   // We have a GET message
                  commandBuffer.remove(0,pos+5);
                  if (DEBUG) Serial.println("\n\r LineBuffer="+commandBuffer);
                  checkVirtuinoCommand(&commandBuffer);
                   if (DEBUG) Serial.println("\n\r response="+responseBuffer);
                  delay(10);
                  client.flush();
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-Type: text/html");
                  client.println("Connection: close");
                  client.println();
                  client.println(responseBuffer);
                  commandBuffer="";
                  responseBuffer="";
                  break;
                } // if pos
             commandBuffer="";
        } // if c=='\n'
    } // if client.available
    delay(1);
    client.stop();  // close the connection:
    if (DEBUG) Serial.println(clientDisconnected);
  }
  }
 
