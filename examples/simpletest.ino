/*
simpletest.ino
Simple example for the HC12 library
*/
#include <SoftwareSerial.h>
#include <HC12.h>

SoftwareSerial HC12Serial(2,3);   // Declare SoftwareSerial for HC12
HC12 hc12(&HC12Serial, 5, 9600);  // Declare HC12: SWSerial address, SET pin, (optional: baud rate)

long int numbers[4] = {1,2,3,4};  // Test array, will be sent to remote HC-12

void setup() {
  Serial.begin(9600);             // Initialize Serial connectioan
  Serial.println("HC-12 library - Simple example");
  hc12.begin();                   // Initialize HC-12 connection
}

void loop() {
  hc12.loop();                    // Send input/output to Serial/HC-12 and run AT commands
  hc12.send("Hello!");            // Send message to remote HC-12. Newline is added automatically.
  hc12.send(numbers, 4);          // Send long int array: array, number of elements
  delay(1000);
}
