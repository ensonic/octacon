// debug logging

#include <debug.h>

Debug::Debug(pin_size_t rx, pin_size_t tx, unsigned long baud, SerialUART *serial) : rx(rx), tx(tx), baud(baud), serial(serial) {}

void Debug::init() {
    if (!serial) return;

    serial->setRX(rx);
    serial->setTX(tx);
    serial->begin(baud);
    while (!serial); // wait for serial attach
}

void Debug::println(const char* s) {
    if (!serial) return;

    serial->println(s);
}

void Debug::printf(const char* fmt, ...) {
    if (!serial) return;

    // meh there is no vprintf, so copy the from Print.cpp (and yes,
    // there is a todo about the author wondering why the need to copy it)
    va_list arg;
    va_start(arg, fmt);
    char temp[80];
    size_t len = vsnprintf(temp, sizeof(temp), fmt, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        // lets simply truncate, this is for logging ...
        temp[sizeof(temp) - 1] = '\0';
    }
    serial->write((const uint8_t*) temp, len);
}