// hardware on pcb-v2

#ifndef SRC_HW_V2_H
#define SRC_HW_V2_H

#include <Arduino.h>
#include <FastCapacitiveSensor.h>
#include <Mux.h>
#include <NeoPixelBus.h>
#include <U8g2lib.h>

#include <config.h>
#include <debug.h>
#include <leds.h>
#include <knobs.h>
#include <midi_io.h>
#include <ui.h>

// Knobs
admux::Pinset addr(D11, D12, D13);
admux::Mux vala(admux::Pin(A0, INPUT, admux::PinType::Analog), addr);
admux::Mux valb(admux::Pin(A1, INPUT, admux::PinType::Analog), addr);
admux::Mux btn(admux::Pin(D10, INPUT_PULLUP, admux::PinType::Digital), addr);
int8_t kmap[] = { 0,1,2,3,7,6,5,4 };
Knobs knobs(&vala, &valb, &btn, kmap);

// LEDs
Leds leds(new LedStripType(numParams/2, D0),new LedStripType(numParams/2, D1));

// OLEDs
// GPIO18 : SCK Serial Clock)
// GPIO19 : SDA (Master Out, Slave In = Serial Data Output)
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled1(U8G2_R0, /* cs=*/ D17, /* dc=*/ D22, /* reset=*/ D16);
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled2(U8G2_R0, /* cs=*/ D21, /* dc=*/ D22, /* reset=*/ D20);
UI ui(&oled1, &oled2);

// USB MIDI object
MidiIO mio;

// will read 4 values, don't ignore any (0.0) and calculate average
FastCapacitiveSensor capsense(D2, A2, 4, 10, 0.0);

// Debugging (disable by passing a nullptr)
Debug dbg(D5, D4,115200UL, &Serial2);

#endif // SRC_HW_V2_H