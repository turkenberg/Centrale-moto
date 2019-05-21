#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // Use 10 for RX, 11 for TX 
 
void setup()
{
   Serial.begin(9600); //open up Serial communication and wait for information 
   while (!Serial) {
    }
 
   Serial.println("Ready!"); 
  
   mySerial.begin(38400); //Set the data rate for the HC-05 Configuration, when in AT mode (comment out next line)
   //mySerial.begin(9600);//Use the following line when communicating Arduino-CHIP (comment out previous line)
 
}
 
void loop() // this is what facilitates the communication
{
   if (mySerial.available())
   Serial.write(mySerial.read());
   if (Serial.available())
   mySerial.write(Serial.read());
}
