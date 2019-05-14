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


#include "VirtuinoEsp8266_WebServer.h"




//======================================================================== VirtuinoEsp8266_WebServer 
//==================================== init ==============================
//========================================================================
#ifdef USE_SOFTWARE_SERIAL
VirtuinoEsp8266_WebServer::VirtuinoEsp8266_WebServer(SoftwareSerial &uart, uint32_t baud):Virtuino::Virtuino(){
    espSerial=&uart;
    espSerial->begin(baud);
    while (espSerial->available()) espSerial->read();
   
   
}

VirtuinoEsp8266_WebServer::VirtuinoEsp8266_WebServer(SoftwareSerial &uart):Virtuino::Virtuino(){
  espSerial=&uart;
  }
#else
VirtuinoEsp8266_WebServer::VirtuinoEsp8266_WebServer(HardwareSerial &uart, uint32_t baud):Virtuino::Virtuino(){
    espSerial=&uart;
    espSerial->begin(baud);
    while (espSerial->available()) espSerial->read();
}

VirtuinoEsp8266_WebServer::VirtuinoEsp8266_WebServer(HardwareSerial &uart):Virtuino::Virtuino(){
  espSerial=&uart;
  }    

#endif


//====================================================================================

void VirtuinoEsp8266_WebServer::run(){
   if(espSerial->available()){
      while (espSerial->available())  {                     
      char c=espSerial->read();
      commandBuffer+=c;
      if (c=='\n') {                                          // check command line to line
          if (DEBUG) Serial.println(commandBuffer);
          int pos = commandBuffer.indexOf("+IPD,");
          if (pos>=0){                                        // check for GET command from Virtuino app
              clearESP_buffer(200);
              int connectionId = commandBuffer.charAt(pos+5)-48;  // get connection ID
              if (DEBUG) Serial.println("ID="+ String(connectionId));
              pos = commandBuffer.indexOf("GET /");
              if (pos!=-1){                                   // We have a GET message
                  commandBuffer.remove(0,pos+5);
                  checkVirtuinoCommand(&commandBuffer);
                  boolean b=wifiSendData(connectionId,&responseBuffer);
                  responseBuffer="";
                  // -----------  Close client connection     -------------
                  clearESP_buffer(50);
             } 
          } 
        commandBuffer="";                                     // clear buffer for the next line
      }//if (c=='\n')
    } // while
   } //if 
}

    
//======================================================================================== connectESP8266_toInternet
//======================================================================================== 
void VirtuinoEsp8266_WebServer::connectESP8266_toInternet(String wifiNetworkName,String wifiNetworkPassword, int port){
    clearESP_buffer(1000);
    if (DEBUG) Serial.println( "Server init...");
    espSerial->println("AT+GMR");
    waitForResponse("OK",1000);
    
    espSerial->println("AT+CWMODE=1");  // configure as client
    waitForResponse("OK",1000);
    
    //--- connect
    espSerial->print("AT+CWJAP=\"");
    espSerial->print(wifiNetworkName);
    espSerial->print("\",\"");
    espSerial->print(wifiNetworkPassword);
    espSerial->println("\"");
    waitForResponse("OK",10000);
     
    espSerial->println("AT+CIFSR");           // get ip address
    waitForResponse("OK",1000);
    
    espSerial->println("AT+CIPMUX=1");         // configure for multiple connections   
    waitForResponse("OK",1000);
    
    espSerial->print("AT+CIPSERVER=1,");
    espSerial->println(port);
    waitForResponse("OK",1000);
   
}
//======================================================================================== createLocalESP8266_wifiServer
//========================================================================================
void  VirtuinoEsp8266_WebServer::createLocalESP8266_wifiServer(String wifiNetworkName,String wifiNetworkPassword, int port, int mode){
      if (DEBUG) Serial.println( "Server init...");
      //if ((mode<2)|| (mode>3)) mode=2; 
      
      clearESP_buffer(1000);
      espSerial->println("AT+GMR");
      waitForResponse("OK",1000);

      espSerial->println("AT+CWMODE=2");  // configure as client
      waitForResponse("OK",1000);

     espSerial->println("AT+CIFSR");
     waitForResponse("OK",1000);

     espSerial->print("AT+CWSAP=\"");
     espSerial->print(wifiNetworkName);
     espSerial->print("\",\"");
     espSerial->print(wifiNetworkPassword);
     espSerial->println("\",2,3");
     waitForResponse("OK",5000);
     
     espSerial->println("AT+CIPMUX=1");         // configure for multiple connections   
     waitForResponse("OK",2000);
    
     espSerial->print("AT+CIPSERVER=1,");
     espSerial->println(port);
     waitForResponse("OK",1000);           
}


//====================================================================== setESP8266_mode
//======================================================================
boolean VirtuinoEsp8266_WebServer::esp8266_setIP(byte a1, byte a2, byte a3, byte a4){
    clearESP_buffer(100);    // clear esp buffer
    espSerial->println("AT+CIPSTA=\""+String(a1)+"."+String(a2)+"."+String(a3)+"."+String(a4)+"\"");
    return waitForResponse("OK",3000);
}

//====================================================================== waitForResponse
//======================================================================
boolean VirtuinoEsp8266_WebServer::waitForResponse(String target1,  int timeout){
    String data;
    char a;
    unsigned long start = millis();
    boolean rValue=false;
    while (millis() - start < timeout) {
        while(espSerial->available() > 0) {
            a = espSerial->read();
            if (DEBUG) Serial.print(a);
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            rValue=true;
            break;
        } 
    }
    return rValue;
}

//=========================================================================== wifiSendData
//===========================================================================
boolean VirtuinoEsp8266_WebServer::wifiSendData(int connectionId, String* content){
  boolean returnedValue=false;
  // HTTP Header
  String httpResponse = "HTTP/1.1 200 OK\r\n";
  httpResponse +=  "Content-Type: text/html; charset=UTF-8\r\n";
  httpResponse += "Content-Length: ";
  httpResponse += content->length();
  httpResponse += "\r\n";
  httpResponse += "Connection: close\r\n\r\n";
  httpResponse += *content;
  httpResponse += (" ");
   //---Send command
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += httpResponse.length();
  cipSend += "\r\n";
  
  for (int i=0;i<cipSend.length();i++) espSerial->write(cipSend.charAt(i));

  if (waitForResponse(">",2000)) {
      for (int i=0;i<httpResponse.length();i++) {
         espSerial->write(httpResponse.charAt(i));
        if (DEBUG) Serial.print(httpResponse.charAt(i));
       }
      if (waitForResponse("SEND OK",10000)) returnedValue=true;
  }
  return returnedValue;
}

//=========================================================================== clearESP_buffer
//===========================================================================
void VirtuinoEsp8266_WebServer::clearESP_buffer(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial->available()){
        c=espSerial->read();
        if (DEBUG) Serial.print(c);
      }
    }
}                    


