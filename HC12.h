#ifndef HC12_h
#define HC12_h
#include <Arduino.h>
#include <SoftwareSerial.h>

class HC12 {

public:
    HC12(uint8_t setPin, int baud);
    HC12(SoftwareSerial *sws, uint8_t setPin, int baud);
    bool begin();
    bool send(char *message);
    bool send(long int data[], int size);
    bool loop();
    bool setOutput(SoftwareSerial *output);
    SoftwareSerial *output;

private:
    uint8_t _setPin;
    bool useSWSerial;
    int _baud;
    const int len = 60;
    bool _isRunning = false;
};

#endif
