/* Virtuino servo motor example (Bluetooth)
 * Created by Ilias Lamprou
 * Updated Oct 21 2016
 * This is the code you need to run on your arduino board to cummunicate whith VirtUino app via bluetooth
 * Before  running this code config the settings below as the instructions on the right
 * 
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 * Getting starting link:
 * Video tutorial link: https://youtu.be/sZHbTwesmnQ
 * Contact address for questions or comments: iliaslampr@gmail.com
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



#include "VirtuinoBluetooth.h"                           // Include VirtuinoBluetooth library to your code

// Code to use SoftwareSerial
// #include <SoftwareSerial.h>                              //  Disable this line if you want to use hardware serial 
// SoftwareSerial bluetoothSerial =  SoftwareSerial(2,3);   // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to bluetooth module TX pin   -  connect the arduino TX pin to bluetooth module RX pin.  Disable this line if you want to use hardware serial 
// VirtuinoBluetooth virtuino(bluetoothSerial);       // Set SoftwareSerial baud rate.  -  Disable this line if you want to use hardware serial 


// Code to use HardwareSerial
 VirtuinoBluetooth virtuino(Serial1);            // enable this line and disable all SoftwareSerial lines
                                                   // Open VirtuinoBluetooth.h file on the virtuino library folder ->  disable the line: #define USE_SOFTWARE_SERIAL 
                                                  


                                              
#include <Servo.h>
Servo myservo;
int oldPos=0;

//================================================================== setup
//==================================================================
//==================================================================
void setup() 
{
  virtuino.DEBUG=true;               // set this value TRUE to enable the serial monitor status
  Serial.begin(9600);                // Set serial monitor baud rate
  
  //bluetoothSerial.begin(9600);       // Disable this line if you want to use software serial
  Serial1.begin(9600);             // Enable this line if you want to use hardware serial
 
  myservo.attach(6);                 // never use vPinMode for this pin 
}


//================================================================== loop
//==================================================================
//==================================================================


void loop(){
   virtuino.run();           //  neccesary command to communicate with Virtuino android app
   
    //------ enter your loop code below here
    //------ avoid to use delay() function in your code
    //------ your code .....
    
    //--- servo motor code
     int pos=virtuino.vMemoryRead(0);         // Read virtuino VM0
     if (pos!=oldPos) {
      myservo.write(pos);  
      Serial.println("pos="+String(pos));
     }
     oldPos=pos;
   
 }


