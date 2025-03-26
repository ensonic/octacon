#include <debug.h>

Debug::Debug(SerialUART *serial) : serial(serial) {}

void Debug::init(pin_size_t rx, pin_size_t tx, unsigned long baud) {
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

    va_list args;
    va_start(args, fmt);
    serial->printf(fmt, args);
    va_end(args);
}