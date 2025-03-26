// debug logging

#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

class Debug {
public:
    Debug(SerialUART *serial=nullptr);

    void init(pin_size_t rx, pin_size_t tx, unsigned long baud = 115200UL);

    void println(const char* s);
    void printf(const char* fmt, ...);

private:
    SerialUART *serial; 
};

#endif // DEBUG_H