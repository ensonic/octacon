// debug logging

#ifndef SRC_DEBUG_H
#define SRC_DEBUG_H

#include <Arduino.h>
#include <config.h>

class Debug {
public:
    Debug(pin_size_t rx, pin_size_t tx, unsigned long baud = 115200UL, SerialUART *serial=nullptr);

    void init();

    void println(const char* s);
    void printf(const char* fmt, ...);

private:
    SerialUART *serial;
    pin_size_t rx, tx;
    unsigned long baud;
};

#endif // SRC_DEBUG_H