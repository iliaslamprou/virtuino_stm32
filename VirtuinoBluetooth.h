/* Virtuino Bluetooth Library for STM32 boards ver 1.62
 * Created by Ilias Lamprou
 * Updated Aug 06 2017
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 */


/*========= Virtuino  public methods to use on your code
 
*  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)   write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
*  int  vDigitalMemoryRead(int digitalMemoryIndex)               read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
*  void vMemoryWrite(int analogMemoryIndex, float value)         write a value to Virtuino float memory       (memoryIndex=0..31, value range as float value)
*  float vMemoryRead(int analogMemoryIndex)                      read the value of  Virtuino analog memory    (analogMemoryIndex=0..31, returned value range = 0..1023)
*  run()                                                         neccesary command to communicate with Virtuino android app  (on start of void loop)
*  int getPinValue(int pin)                                      read the value of a Pin. Usefull for PWM pins
*  void clearTextBuffer();                                       Clear the text received text buffer
*  int textAvailable();                                          Check if there is text in the received buffer
*  String getText(byte ID);                                      Read the text from Virtuino app
*  void sendText(byte ID, String text);                          Send text to Virtuino app
*  void vDelay(long milliseconds);
*  boolean DEBUG=false;                                          
*  long lastCommunicationTime;
*/

#ifndef VirtuinoBluetooth_h
#define VirtuinoBluetooth_h



#include "Arduino.h"
#include "Virtuino.h"
#ifdef USE_SOFTWARE_SERIAL
#include "SoftwareSerial.h"
#endif



//====================================================================
 class VirtuinoBluetooth : public Virtuino {
  public:                                            
  #ifdef USE_SOFTWARE_SERIAL
    VirtuinoBluetooth(SoftwareSerial &uart);
    VirtuinoBluetooth(SoftwareSerial &uart, uint32_t baud);
  #else 
    VirtuinoBluetooth(HardwareSerial &uart);
    VirtuinoBluetooth(HardwareSerial &uart, uint32_t baud);
  #endif
   virtual void run();
   
  private:
   void sendCommandResponse(char commandType, int commandPin , String commandValueString);
   void checkIfIOsHaveChanged();
   void  readBluetoothSerialData();
   virtual String urlencode(String* str);
   virtual String urldecode(String* str); 
   #ifdef USE_SOFTWARE_SERIAL
      SoftwareSerial *BTserial;
  #else
      HardwareSerial *BTserial;
  #endif  
 
 };



#endif

