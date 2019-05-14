/* Virtuino Kernel Library for STM32 boards ver 1.63
 * Created by Ilias Lamprou  & Vincenzo Gibiino
 * Updated Noe 24 2017
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


#ifndef Virtuino_h
#define Virtuino_h



//#define USE_SOFTWARE_SERIAL   // disable this line if you want to use hardware serial
                              // disable this command for Arduino DUE, STM32


//=================   SELECT YOUR BOARD ======= 

// #define NODEMCU                       // Enable this line to use Virtuino library with ESP8266 modules like  NodeMCU, Wemos, etc.  



 #if defined (ARDUINO_GENERIC_STM32F103T)
  #define STM32PACKAGE 36
#elif defined (ARDUINO_GENERIC_STM32F103C)
  #define STM32PACKAGE 48
#elif defined (ARDUINO_GENERIC_STM32F103R)
  #define STM32PACKAGE 64
#elif defined (ARDUINO_GENERIC_STM32F103V)
  #define STM32PACKAGE 100
#elif defined (ARDUINO_GENERIC_STM32F103Z)
  #define STM32PACKAGE 144
#elif defined (ARDUINO_MAPLE_MINI)
  #define STM32PACKAGE MM
#endif

//mdf
 #ifndef STM32PACKAGE
    #warning no board selected, see virtuino.h
    #define STM32PACKAGE 144    //choose your MCU package 48, 64, 100 or 144 pin if your board is not in the #define above
 #endif
 
 
  
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
      #define analogInputPinsCount 16  
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15 }
      #define boardPinsCount  54                  
      #define digitalInputPinsMap_ {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53}

#elif defined(ARDUINO_AVR_NANO)
      #define analogInputPinsCount 8 
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5, A6, A7}
      #define boardPinsCount  14                      // max pins of all arduino boards
      #define digitalInputPinsMap_ {0,1,2,3,4,5,6,7,8,9,10,11,12,13}

#elif defined(WEMOS_D1_MINI) || defined(NODEMCU)
      #define analogInputPinsCount 1 
      #define analogInputPinsMap_ {A0}
      #define boardPinsCount  11                 
      #define digitalInputPinsMap_ {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10}
    
#elif defined (ARDUINO_ARCH_STM32F1)

  
    #if STM32PACKAGE == 144	//ARDUINO_GENERIC_STM32F103Z

    #define analogInputPinsCount 16 
    #define analogInputPinsMap_ {PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PB0,PB1,PC0,PC1,PC2,PC3,PC4,PC5}	//same as 64pin
    #define boardPinsCount  93  //to correct                  
    #define digitalInputPinsMap_ {PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15,
                                  PB3,PB4,PB5,PB6,PB7,PB8,PB9,PB10,PB11,PB12,PB13,PB14,PB15,
                                  PC6,PC7,PC8,PC9,PC10,PC11,PC12,PC13,PC14,PC15,
                                  PD2,PD3,PD4,PD5,PD6,PD7,PD8,PD9,PD10,PD11,PD12,PD13,PD14,PD15,
                                  PE0,PE1,PE2,PE3,PE4,PE5,PE6,PE7,PE8,PE9,PE10,PE11,PE12,PE13,PE14,PE15,
                                  PF0,PF1,PF2,PF3,PF4,PF5,PF6,PF7,PF8,PF9,PF10,PF11,PF12,PF13,PF14,PF15,
                                  PG0,PG1,PG2,PG3,PG4,PG5,PG6,PG7,PG8,PG9,PG10,PG11,PG12,PG13,PG14,PG15}
    //PWM: PA0,PA1,PA2,PA3,PA6,PA7,PA8,PA9,PA10,PB0,PB1,PB6,PB7,PB8,PB9,PC6,PC7,PC8,PC9 same as 64pin
	
    #elif STM32PACKAGE == 100	//ARDUINO_GENERIC_STM32F103V
    
    #define analogInputPinsCount 16 
    #define analogInputPinsMap_ {PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PB0,PB1,PC0,PC1,PC2,PC3,PC4,PC5}	//same as 64pin
    #define boardPinsCount  61      //to correct                
    #define digitalInputPinsMap_ {PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15, //8
                                  PB3,PB4,PB5,PB6,PB7,PB8,PB9,PB10,PB11,PB12,PB13,PB14,PB15, //13
                                  PC6,PC7,PC8,PC9,PC10,PC11,PC12,PC13,PC14,PC15, //10
                                  PD2,PD3,PD4,PD5,PD6,PD7,PD8,PD9,PD10,PD11,PD12,PD13,PD14,PD15, //14
                                  PE0,PE1,PE2,PE3,PE4,PE5,PE6,PE7,PE8,PE9,PE10,PE11,PE12,PE13,PE14,PE15} //16
    //PWM:PA0,PA1,PA2,PA3,PA6,PA7,PA8,PA9,PA10,PB0,PB1,PB6,PB7,PB8,PB9,PC6,PC7,PC8,PC9 same as 64pin
	
 #elif STM32PACKAGE == 64	//ARDUINO_GENERIC_STM32F103R
    #define analogInputPinsCount 16 
    #define analogInputPinsMap_ {PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PB0,PB1,PC0,PC1,PC2,PC3,PC4,PC5}	
    #define boardPinsCount  32                    
    #define digitalInputPinsMap_ {PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15,PB3,PB4,PB5,PB6,PB7,PB8,PB9,PB10,PB11,PB12,PB13,PB14,PB15,PC6,PC7,PC8,PC9,PC10,PC11,PC12,PC13,PC14,PC15,PD2}
	//PWM:  PA0,PA1,PA2,PA3,PA6,PA7,PA8,PA9,PA10,PB0,PB1,PB6,PB7,PB8,PB9,PC6,PC7,PC8,PC9
	
  #elif STM32PACKAGE == 48	//ARDUINO_GENERIC_STM32F103C
    #define analogInputPinsCount 10 
    #define analogInputPinsMap_ {PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PB0,PB1}
    #define boardPinsCount  24                    
    #define digitalInputPinsMap_ {PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15,PB3,PB4,PB5,PB6,PB7,PB8,PB9,PB10,PB11,PB12,PB13,PB14,PB15,PC13,PC14,PC15}   
    //PWM: PB0, PA7, PA6, PA3, PA2, PA1, PA0, PB7, PB6, PA10, PA9, PA8
                                 
  #elif STM32PACKAGE == 36	//ARDUINO_GENERIC_STM32F103T
    #define analogInputPinsCount 10 
    #define analogInputPinsMap_ {PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PB0,PB1}
    #define boardPinsCount  13            
    #define digitalInputPinsMap_ {PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15,PB3,PB4,PB5,PB6,PB7}    
    //PWM: PB0, PA7, PA6, PA3, PA2, PA1, PA0, PB7, PB6, PA10, PA9, PA8
    
  #elif STM32PACKAGE == MM
    #define analogInputPinsCount 9
    #define analogInputPinsMap_ {3, 4, 5, 6, 7, 8, 9, 10, 11}
    #define boardPinsCount  23                  
    #define digitalInputPinsMap_ {0,1,2, 12,13,14, 15, 16,17,18,19,20,21,22,23,24, 25, 26, 27,28,29,30,31} 
    //PWM:  3, 4, 5, 8, 9, 10, 11, 15, 16, 25, 26, 27
    
  #endif //STM32PACKAGE
      
#else
      #define analogInputPinsCount 6 
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5}
      #define boardPinsCount  14           
      #define digitalInputPinsMap_ {0,1,2,3,4,5,6,7,8,9,10,11,12,13}
     
#endif

#include "Arduino.h"




#define firmwareCode  "!C00=1.62$"           // This firmware version

#define virtualDigitalMemorySize  32            // Size of virtual pins DV type integer
#define virtualAnalogMemorySize  32             // Size of virtual pins V type float



#define ERROR_PIN       1
#define ERROR_VALUE     2
#define ERROR_TYPE      3
#define ERROR_SIZE      4
#define ERROR_PASSWORD  5
#define ERROR_COMMAND   6
#define ERROR_UNKNOWN   7

#define COMMAND_START_CHAR '!'                 // All Virtuino commands starts with !
#define COMMAND_END_CHAR   '$'                 // All Virtuino commands ends with $
#define COMMAND_ERROR     "E00="           

#define TEXT_COMMAND_MAX_SIZE 80               // Set the max supported size of text commands
//====================================================================
 class Virtuino {
  public:                                            
    Virtuino();
    virtual  void run();
    void vDigitalMemoryWrite(int digitalMemoryIndex, int value);
    int  vDigitalMemoryRead(int digitalMemoryIndex);
    void vMemoryWrite(int memoryIndex, float value);
    float vMemoryRead(int memoryIndex);
    int getPinValue(int pin);                             // Useful to read the value of a PWM pin 
    void vPinMode(int pin, int mode);                     
    void vDelay(long milliseconds);

    long lastCommunicationTime;                           // The time in millis of last communication with Virtuino app
    boolean DEBUG=false;
    String password=""; 

    
    void  checkVirtuinoCommand(String* command);    
    char  getCommandType(char c);
    int  getCommandPin(String* aCommand);
    float getCommandValue(String* aCommand);
    int arduinoPinsValue[boardPinsCount];
    int virtualDigitalMemoryIdol[virtualDigitalMemorySize];     // Only for bluetooth library
    int virtualDigitalMemory[virtualDigitalMemorySize];  
    byte arduinoPinsState[boardPinsCount];                     // used by bluetooth library only
    int  getBoardDigitalPin(int pinIndex);
    
    boolean newCommand=false;                                        // This flag takes the value 1 every time we have a commplete command from app
    String commandBuffer;                                            // Stores the active command received from app
    String responseBuffer;                                           // Stores the reply from board to virtuino app
     //-- Text Command functions
    void clearTextBuffer();
    int textAvailable();
    String getText(byte ID);
    void sendText(byte ID, String text);
    String textToSendCommandBuffer="";
  
    void  executeReceivedCommand(char activeCommandType, int activeCommandPin ,String* textValue, boolean returnInfo);
    
   private:
     
     String getErrorCommand(byte code);
     
     int  getBoardAnalogPin(int analogPinIndex);
     float virtualFloatMemory[virtualAnalogMemorySize];                         
     
     byte analogInputPinsMap[analogInputPinsCount]  = analogInputPinsMap_  ;
     byte digitalInputPinsMap[boardPinsCount]=digitalInputPinsMap_;                     // used by bluetooth library only

    //-- Text Command functions
      String textReceivedCommandBuffer="";
      virtual String urlencode(String* str);
      unsigned char h2int(char c);
      virtual String urldecode(String* str); 
      void clearTextByID(byte ID, String* textBuffer);
      void addTextToReceivedBuffer(byte ID, String* text);
   
 };

#endif

