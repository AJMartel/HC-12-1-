#include <Arduino.h>
#include "HC12.h"

HC12::HC12(uint8_t setPin, int baud = 9600) {
  _baud = baud;
  useSWSerial = false; //Will be used in the future
}

HC12::HC12(SoftwareSerial *sws, uint8_t setPin, int baud = 9600) {
  _baud = baud;
  this->setOutput(sws);
}

bool HC12::setOutput(SoftwareSerial *sws){
  this->output = sws;
  useSWSerial = true;
  return true;
}

//@todo: Verify if HC-12 is responding by sending AT and receiving OK
bool HC12::begin(){
  pinMode(_setPin, OUTPUT);
  digitalWrite(_setPin, HIGH);
  delay(80);
  if (useSWSerial){
    this->output->begin(_baud);
  }
  _isRunning = true;
  return true;
}

bool HC12::send(char message[]){
  if (strncmp(message, "AT", 2) == 0) {
    this->output->print(message);  // Send command to remote HC-12
    delay(100);
    digitalWrite(_setPin, LOW);    // Command mode
    delay(100);
    Serial.print(message);
    this->output->print(message);  // Send command to local HC-12
    delay(500);                    // Wait for reply
    digitalWrite(_setPin, HIGH);   // Exit command / transparent mode
    delay(100);
  }
  else {
    this->output->println(message);
  }
  return true;
}

//@todo: Split to two (or more) messages if too long
bool HC12::send(long int data[], int size){
  char message[len];
  uint8_t cursor = 0;

  for (uint8_t i = 0; i < size; i++) {
    sprintf(message + cursor, "%ld;", data[i]); // %ld = long int
    while (message[cursor] && cursor < len){
      cursor++;
    }
  }
  if (this->send(message)){
    return true;
  }
  return false;
}


bool HC12::loop(){
  char HC12ByteIn;
  char SerialByteIn;
  char HC12ReadBuffer[len];
  char SerialReadBuffer[len];
  bool HC12End;
  bool SerialEnd;
  uint8_t i = 0;
  uint8_t j = 0;

  while (this->output->available()){            // While Arduino's HC12 RX buffer has data
    HC12ByteIn = this->output->read();          // Store each character in byteIn
    HC12ReadBuffer[i] = char(HC12ByteIn);       // Write each character of byteIn to HC12ReadBuffer
    if (HC12ByteIn == '\n'){
      HC12End = true;
    }
    i++;
  }

  HC12ReadBuffer[i] = '\0';

  while (Serial.available()){                   // While Arduino's Serial RX buffer has data
    SerialByteIn = Serial.read();               // Store each character in byteIn
    SerialReadBuffer[i] = char(SerialByteIn);   // Write each character of byteIn to SerialReadBuffer
    if (SerialByteIn == '\n'){
      SerialEnd = true;
    }
    j++;
  }

  SerialReadBuffer[j] = '\0';

  if (SerialEnd) {
    if (strncmp(SerialReadBuffer, "AT", 2) == 0){
      this->output->print(SerialReadBuffer);    // Send command to remote HC-12
      delay(100);
      digitalWrite(_setPin, LOW);               // Command mode
      delay(100);
      Serial.print(SerialReadBuffer);
      this->output->print(SerialReadBuffer);    // Send command to local HC-12
      delay(500);                               // Wait for reply
      digitalWrite(_setPin, HIGH);              // Exit command / transparent mode
      delay(100);
    }
    else {
      this->output->print(SerialReadBuffer);
    }
  }

  if (HC12End) {
    if (strncmp(HC12ReadBuffer, "AT", 2) == 0) {
      digitalWrite(_setPin, LOW);               // Command mode
      delay(100);
      Serial.print(SerialReadBuffer);
      this->output->print(HC12ReadBuffer);      // Send command to local HC-12
      delay(500);                               // Wait  for reply
      digitalWrite(_setPin, HIGH);              // Exit command / transparent mode
      delay(100);
      this->output->println("Remote Command Executed");  // Acknowledge execution
    }
    else {
      Serial.print(HC12ReadBuffer);             // Send message to screen
    }
  }

  return true;
}
