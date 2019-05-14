/* Virtuino Kernel Library for STM32 boards ver 1.62
 * Created by Ilias Lamprou  & Vincenzo Gibiino
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

#include "Virtuino.h"




//========================================================================= Virtuino init
//=========================================================================

 Virtuino::Virtuino(){}

//========================================================================= run
//=========================================================================
 void    Virtuino::run(){}


//========================================================================= checkNetworkCommand
//======================================================================================
void  Virtuino::checkVirtuinoCommand(String* command){
     int pos=command->lastIndexOf(COMMAND_END_CHAR);                      
  if (pos>5){                                          // check the size of command
      command->remove(pos+1);     // clear the end of command buffer
     //------------------  get command password
      String commandPassword="";
      int k=command->indexOf(COMMAND_START_CHAR);
       if (k>0) {
        commandPassword = command->substring(0,k);
        command->remove(0,k);   // clear the start part of the command  buffer
       }
      responseBuffer="";
      if ((password.length()==0) || (commandPassword.equals(password))) {                       // check if password correct
           while (command->indexOf(COMMAND_START_CHAR)>=0){
              int cStart=command->indexOf(COMMAND_START_CHAR);
              int cEnd=command->indexOf(COMMAND_END_CHAR);
              if (cEnd-cStart>5){
                String oneCommand = command->substring(cStart+1,cEnd);                               // get one command
                char commandType = getCommandType(oneCommand.charAt(0));
                  if (commandType!='E') {
                     int pin= getCommandPin(&oneCommand);
                     if (pin!=-1){
                        boolean returnInfo=false;
                        if (oneCommand.charAt(4)=='?') returnInfo=true;
                        oneCommand.remove(0,4);
                        if (oneCommand.length()<TEXT_COMMAND_MAX_SIZE) executeReceivedCommand(commandType,pin,&oneCommand,returnInfo);
                        else {
                          responseBuffer+=getErrorCommand(ERROR_SIZE);
                          String s="";
                          if (pin<9) s+="0";
                          s+=String(pin);
                          responseBuffer+="!T";
                          responseBuffer+=s;
                          responseBuffer+="= $";           // return an empty text if the size of text command is too big
                          
                        }
                        lastCommunicationTime=millis();
                     } else  responseBuffer +=getErrorCommand(ERROR_PIN);  // response  error pin number   !E00=1$   
                  } else responseBuffer +=getErrorCommand(ERROR_TYPE);  // response  error type   !E00=3$   
          
              } else{
                responseBuffer+=getErrorCommand(ERROR_SIZE);  // response  error size of command   !E00=4$   
              }
              command->remove(0,cEnd+1-cStart);
           }  // while
      } else responseBuffer+=getErrorCommand(ERROR_PASSWORD);     // the password is not correct
  }
  else  responseBuffer+=getErrorCommand(ERROR_SIZE);         // the size of command is not correct

    if (textToSendCommandBuffer.length()>0) {
      responseBuffer+=textToSendCommandBuffer;
      textToSendCommandBuffer="";
    }
  
}








   //================================================================= getCommandType
   //=================================================================
  //This function returns the command type which was received from app
  //The second character of command string determines the command type
  // I  digital input read command    
  // Q  digital ouput write
  // D  digital (integer) memory read - write commang
  // A  analog input analog input read command
  // O  analog output write command
  // V  float memory read - write command
  // C  get connection info
  // T  text
  // E  error
  // Other characters are recognized as an error and function returns the character E
  
   char  Virtuino::getCommandType(char c){
    if (!(c== 'I' || c == 'Q' || c == 'D' ||c == 'A' ||c == 'O' || c == 'V' || c == 'C' || c == 'T')){
      c='E'; //error  command
    }
    return c;
  }
  //================================================================= getCommandPin
  //=================================================================
  //This function returns the pin number of the command string which was received from app
  //Fuction checks if pin number is correct. If not it returns -1 as error code
  //This is a system fuction. Don't change this code
 
  int  Virtuino::getCommandPin(String* aCommand){
    char p1= aCommand->charAt(1);
    char p2= aCommand->charAt(2);
    String s="-1";
    if (isDigit(p1) && isDigit(p2)) {
       s="";
       s+=p1;
       s+=p2;
    }
     return s.toInt();
  }

//================================================================= getCommandValue
float Virtuino::getCommandValue(String* aCommand){
     boolean er=false;
     for (int i=0;i<aCommand->length();i++){
        char c=aCommand->charAt(i);
        if (! ((c=='.') || (c=='-') || (isDigit(aCommand->charAt(i))))) er=true;
       }
      if (er==false) return aCommand->toFloat();
    
    return 0;
  }
//================================================================= getErrorCommand
//=================================================================
String Virtuino::getErrorCommand(byte code){
  String s="";
  s+=COMMAND_START_CHAR;
  s+=COMMAND_ERROR;
  s+=code;
  s+=COMMAND_END_CHAR;
  return s;
}


//================================================================================== urlencode
//==================================================================================
String Virtuino::urlencode(String* str){
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str->length(); i++){
      c=str->charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}

//================================================================================== h2int
//==================================================================================
unsigned char Virtuino::h2int(char c){
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}
//================================================================================== urldecode
//==================================================================================
String Virtuino::urldecode(String* str){
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str->length(); i++){
        c=str->charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str->charAt(i);
        i++;
        code1=str->charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
         encodedString+=c;  
      }
      yield();
    }
    
   return encodedString;
}

//======================================================================================== clearTextBuffer
//========================================================================================
void Virtuino:: clearTextBuffer(){textReceivedCommandBuffer="";}

//======================================================================================== clearTextBuffer
//========================================================================================
int Virtuino::  textAvailable(){return textReceivedCommandBuffer.length();}
  
//======================================================================================== getTextByID
//========================================================================================
String Virtuino:: getText(byte ID){
   String returnedText="";
   String ID_string ="";
   ID_string+=COMMAND_START_CHAR;
   ID_string+='T';
   if (ID<10) ID_string+='0';
   ID_string+=ID;
   ID_string+="=";       
   int  pos=textReceivedCommandBuffer.indexOf(ID_string);
   if (pos>=0) {
      pos= textReceivedCommandBuffer.indexOf("=",pos);
      int lastPos=textReceivedCommandBuffer.indexOf(COMMAND_END_CHAR,pos);
      returnedText= textReceivedCommandBuffer.substring(pos+1,lastPos);
      returnedText=urldecode(&returnedText);
      clearTextByID(ID,&textReceivedCommandBuffer);
   }
   return returnedText;
  }

//======================================================================================== clearTextByID
//========================================================================================
void Virtuino:: clearTextByID(byte ID, String* textBuffer){
   String ID_string ="";
   ID_string+=COMMAND_START_CHAR;
   ID_string+='T';
   if (ID<10) ID_string+='0';
   ID_string+=ID;
   ID_string+="=";                   // !ID3=
   int  pos=textBuffer->indexOf(ID_string);
   if (pos>=0) {
      int lastPos= textBuffer->indexOf(COMMAND_END_CHAR,pos);
      textBuffer->remove(pos,lastPos+1);
   }
}

//======================================================================================== addTextToBuffer
//========================================================================================
void Virtuino::addTextToReceivedBuffer(byte ID, String* text){
   clearTextByID(ID,&textReceivedCommandBuffer);
   textReceivedCommandBuffer+=COMMAND_START_CHAR;
   textReceivedCommandBuffer+= 'T';
   if (ID<10) textReceivedCommandBuffer+= '0';
   textReceivedCommandBuffer+=ID;
   textReceivedCommandBuffer+= "=";
   textReceivedCommandBuffer+= *text;
   textReceivedCommandBuffer+=COMMAND_END_CHAR;
}

//======================================================================================== addTextToBuffer
//========================================================================================
void Virtuino::sendText(byte ID, String text){
   clearTextByID(ID,&textToSendCommandBuffer);
   textToSendCommandBuffer+=COMMAND_START_CHAR;
   textToSendCommandBuffer+= 'T';
   if (ID<10) textToSendCommandBuffer+= '0';
   textToSendCommandBuffer+=ID;
   textToSendCommandBuffer+= "=";
   textToSendCommandBuffer+=urlencode(&text); 
   textToSendCommandBuffer+=COMMAND_END_CHAR;
  
}

//================================================================= executeCommandFromApp
//=================================================================
   void  Virtuino::executeReceivedCommand(char activeCommandType, int activeCommandPin ,String* textValue, boolean returnInfo){
   // The value activeCommandType contains command type such as Digital output, Analog output etc.
   // The value activeCommandPin contains the pin number of the command
   // The value activeCommandValue contains the value (0,1 or 2 for digital, 0-255 for analog outputs, 0-1023 for analog memory) 
   // In this void we have to check if the command is correct and then execute the command 
   // After executing the command we have to send a response to Virtuinio app
 
   String response=getErrorCommand(ERROR_UNKNOWN); 
    String pinString="";
    int boardPin=-1;
    float activeCommandValue=0;
    
    if (activeCommandPin<10) pinString = "0"+String(activeCommandPin);
    else pinString=String(activeCommandPin);
     
   
    switch (activeCommandType) {
         
      case 'T':                         
            if ((activeCommandPin>=0) & (activeCommandPin < 100)){
                response="";
                response +=COMMAND_START_CHAR;
                response +=activeCommandType;
                response +=pinString;
                response +='=';
                response +=*textValue;
                response +=COMMAND_END_CHAR;  // response 
                addTextToReceivedBuffer(activeCommandPin,textValue);
            }
          break;
         
      case 'I':                         
              boardPin=getBoardDigitalPin(activeCommandPin);
              if (boardPin!=-1)
              response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(boardPin))+COMMAND_END_CHAR;  // response 
            else  response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"=2"+COMMAND_END_CHAR;  // response 
          break;
          
      case 'Q': 
              boardPin=getBoardDigitalPin(activeCommandPin);
              if (boardPin!=-1){
                 if (returnInfo) response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(boardPin))+COMMAND_END_CHAR;  // response 
                 else {
                   activeCommandValue = getCommandValue(textValue);
                   if ((activeCommandValue == 0) || (activeCommandValue == 1)) {
                          digitalWrite(boardPin,activeCommandValue);
                          arduinoPinsValue[activeCommandPin]=activeCommandValue;
                          response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(boardPin))+COMMAND_END_CHAR;  // response 
                   } else    response =getErrorCommand(ERROR_VALUE);
                 }
            }  else  response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"=0"+COMMAND_END_CHAR;  // response 
          break; 

       case 'D':
            if ((activeCommandPin>=0) & (activeCommandPin<virtualDigitalMemorySize)){ 
                if (returnInfo) response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualDigitalMemory[activeCommandPin])+COMMAND_END_CHAR;  // response 
                else{
                     activeCommandValue= getCommandValue(textValue);
                      virtualDigitalMemory[activeCommandPin]= activeCommandValue; 
                      virtualDigitalMemoryIdol[activeCommandPin] = activeCommandValue;
                      response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualDigitalMemory[activeCommandPin])+COMMAND_END_CHAR;  // response 
                }
            } else   response =getErrorCommand(ERROR_PIN);  // response  error pin number   !E00=1$   
            break; 
      
       case 'V': 
           if ((activeCommandPin>=0) & (activeCommandPin<virtualAnalogMemorySize)){
               if (returnInfo) response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualFloatMemory[activeCommandPin])+COMMAND_END_CHAR;  // response
               else { 
                   activeCommandValue= getCommandValue(textValue);
                    virtualFloatMemory[activeCommandPin]= activeCommandValue; 
                    response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualFloatMemory[activeCommandPin])+COMMAND_END_CHAR;  // response
                    } 
           } else   response =getErrorCommand(ERROR_PIN);
          break;
       case 'O': 
             boardPin=getBoardDigitalPin(activeCommandPin);
              if (boardPin!=-1){
                   arduinoPinsState[activeCommandPin]=3; 
                   if (returnInfo) {
                      int pwm_value = pulseIn(boardPin, HIGH);
                      pwm_value = pwm_value /7.85;
                      response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(pwm_value)+COMMAND_END_CHAR;  // response 
                  }
                   else {
                          activeCommandValue= getCommandValue(textValue);
                            arduinoPinsValue[activeCommandPin]=(int) activeCommandValue;
                            analogWrite(boardPin,(int)activeCommandValue);
                            response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String((int)activeCommandValue)+COMMAND_END_CHAR;  // response 
                        }
           } else    response =COMMAND_START_CHAR+String(activeCommandType)+pinString+"=0"+COMMAND_END_CHAR;  // response 
          break;

         case 'A':                        
             boardPin=getBoardAnalogPin(activeCommandPin);
              if (boardPin!=-1)
              response ="!"+String(activeCommandType)+pinString+"="+String(analogRead(boardPin))+"$";  // response 
            else   response =getErrorCommand(ERROR_PIN);  // response  error pin number   !E00=1$       
          break;  

           
           case 'C':                         
                     activeCommandValue= getCommandValue(textValue);
                     if (activeCommandValue==1) response =firmwareCode;     // return firmware version
                      break;

  
    }
      
     responseBuffer += response;
     
  }

//=================================================================== getBoardDigitalPin
//===================================================================
int  Virtuino::getBoardDigitalPin(int pinIndex){
  if (pinIndex<boardPinsCount) return digitalInputPinsMap[pinIndex];
  //if (pinIndex<boardPinsCount) return pinIndex;
  else return -1;                                         // error pin number is not correct
}
//=================================================================== getBoardAnalogPin
//===================================================================
int  Virtuino::getBoardAnalogPin(int analogPinIndex){
  if (analogPinIndex<analogInputPinsCount) return analogInputPinsMap[analogPinIndex];
  else return -1;                                         // error pin number is not correct
}

//=================================================================== getPinValue
//===================================================================
int Virtuino::getPinValue(int pin){
  if (pin>=0 && pin<boardPinsCount) return arduinoPinsValue[pin];
  else return 0;
  }
  //=================================================================== vmDigitalWrite
  //===================================================================
 void Virtuino::vDigitalMemoryWrite(int digitalMemoryIndex, int value){
  if ((digitalMemoryIndex>=0) && (digitalMemoryIndex<virtualDigitalMemorySize)){
    virtualDigitalMemory[digitalMemoryIndex]=value;
  }
  
}
//=================================================================== vmDigitalRead
//===================================================================
 int Virtuino::vDigitalMemoryRead(int digitalMemoryIndex){
  if ((digitalMemoryIndex>=0) & digitalMemoryIndex<virtualDigitalMemorySize){
    return virtualDigitalMemory[digitalMemoryIndex];
  }
  else return 0;    // error
}

//=================================================================== vmAnalogWrite
//===================================================================
// This void must not be called more than two times per second
void Virtuino::vMemoryWrite(int memoryIndex, float value){
  if ((memoryIndex>=0) & memoryIndex<virtualAnalogMemorySize){
      virtualFloatMemory[memoryIndex]=value;
  }
}

//=================================================================== vmAnalogRead
//===================================================================
 float Virtuino::vMemoryRead(int memoryIndex){
  if ((memoryIndex>=0) & memoryIndex<virtualAnalogMemorySize){
    return virtualFloatMemory[memoryIndex];
  }
  else return 0;    // error
}

//=================================================================== vPinMode
void Virtuino::vPinMode(int pin, int mode){
#ifdef ARDUINO_ARCH_STM32F1
  WiringPinMode pin_mode = (WiringPinMode) mode;
#else
int pin_mode = mode;
#endif
    pinMode(pin,pin_mode);
    for (int i=0;i<boardPinsCount;i++){
      if (pin==digitalInputPinsMap[i]){
        if (mode == OUTPUT ) arduinoPinsState[pin] =  1;
        else if (mode == INPUT ) arduinoPinsState[pin] =  2;
        return;
      }
    }
 }



//=================================================================== vDelay
//===================================================================
void Virtuino::vDelay(long milliseconds){
  long timeStart=millis();
  while (millis()-timeStart<milliseconds) run();
}


