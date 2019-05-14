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
 
#include "VirtuinoBluetooth.h"

  

//======================================================================== VirtuinoBluetooth 
//==================================== init ==============================
//========================================================================
#ifdef USE_SOFTWARE_SERIAL
VirtuinoBluetooth::VirtuinoBluetooth(SoftwareSerial &uart, uint32_t baud):Virtuino::Virtuino(){
    BTserial=&uart;
    BTserial->begin(baud);
    while (BTserial->available()) BTserial->read();
}

VirtuinoBluetooth::VirtuinoBluetooth(SoftwareSerial &uart):Virtuino::Virtuino(){
  BTserial=&uart;
  }
#else
VirtuinoBluetooth::VirtuinoBluetooth(HardwareSerial &uart, uint32_t baud):Virtuino::Virtuino(){
    BTserial=&uart;
    BTserial->begin(baud);
    while (BTserial->available()) BTserial->read();
}

VirtuinoBluetooth::VirtuinoBluetooth(HardwareSerial &uart):Virtuino::Virtuino(){
  BTserial=&uart;
  }    

#endif

//============================================================== run
//==============================================================
 void    VirtuinoBluetooth:: VirtuinoBluetooth::run(){
   checkIfIOsHaveChanged();                                // This void informs VirtUino app every time a pin's status is changed 
  if (BTserial->available()) readBluetoothSerialData();    // read serial buffer
}

//======================================================================== readBluetoothSerialData
//========================================================================
//This void reads data from bluetooth serial and stores them to commandBuffer
//If we have a valid  command from app, void then calls the void executeReceivedCommand to execute the command
//If command doesn't have a valid format it returns an error response to App  - error code = 0

void VirtuinoBluetooth::readBluetoothSerialData(){
        
        while (BTserial->available()) {
             char tempChar=BTserial->read();
             //if (DEBUG) Serial.print(tempChar);
             if (tempChar==COMMAND_START_CHAR) {                           // a new command is starting...
             if (newCommand==false) newCommand=true;   
             else sendCommandResponse('E',0,String(ERROR_COMMAND));
             commandBuffer="";                             //clear buffer for the new command
        }
        if (newCommand){                                // add serial charracters to  new command
            commandBuffer+=tempChar; 
            if (commandBuffer.length() > 150){             //error  while reading data
              newCommand=false;                           // cancel active command 
              commandBuffer="";                           // clear buffer for the next command
              sendCommandResponse('E',0,String(ERROR_COMMAND));
              break;
            } 
            if (tempChar==COMMAND_END_CHAR){                          // end of command
                checkVirtuinoCommand(&commandBuffer);
                if (DEBUG) Serial.print("\r\nResponse= "+responseBuffer+"\r\n");
                BTserial->print(responseBuffer);
                newCommand=false;
                commandBuffer="";
                responseBuffer="";
                break;
            }
        }}
  }
//==================================================================  sendCommandResponse  
//================================================================== 
 void  VirtuinoBluetooth::sendCommandResponse(char commandType, int commandPin , String commandValueString){
  // command format examples  +Q13=0001#    +I05=000?  +D10=0000#  +A03=000?#   +O10=0090#   +V02=0456#
    String s="";
    s+=COMMAND_START_CHAR;
    s+=commandType;
    if (commandPin<10) s+='0';
    s+=commandPin;
    s+='=';
    s+=commandValueString;
    s+=COMMAND_END_CHAR;
    if (DEBUG) Serial.print("\r\nResponse$= "+s+"\r\n");
    BTserial->print(s);
   
    delay(10);              
}



//================================================================= checkForIOsChanges
//=================================================================
void VirtuinoBluetooth::checkIfIOsHaveChanged(){
   int pin;
    int p;
    int pinValue;
        //---- check if digitalInput state has changed
         for (int i=0;i<boardPinsCount; i++){
           if ((arduinoPinsState[i]>0) && (arduinoPinsState[i]<3)) {
              byte truePin=getBoardDigitalPin(i);
              pinValue=digitalRead(truePin);
              if (arduinoPinsValue[i]!=pinValue){
                  arduinoPinsValue[i]=pinValue;
                  if ((arduinoPinsState[i]==1) || (arduinoPinsState[i]==2)) sendCommandResponse('Q',i,String(pinValue));
        //          delay(10);
               }
           }
         } // end for  
        
       //---- check if digitalMemory has changed
          for (int i=0;i<virtualDigitalMemorySize; i++){
           if (virtualDigitalMemoryIdol[i] != virtualDigitalMemory[i]){
              virtualDigitalMemoryIdol[i] = virtualDigitalMemory[i];
              sendCommandResponse('D',i,String(virtualDigitalMemory[i]));
           } 
          }

}




//================================================================================== urlencode
//==================================================================================
String VirtuinoBluetooth::urlencode(String* str){
      str->replace("!","%21");
      str->replace("$","%24");
      return *str;
}
//================================================================================== urldecode
//==================================================================================
String VirtuinoBluetooth::urldecode(String* str){
    str->replace("%21","!");
    str->replace("%24","$");
    return *str;
}

