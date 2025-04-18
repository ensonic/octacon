#include <Arduino.h>
#include <Mux.h>
#include <NeoPixelBus.h>
#include <U8g2lib.h>

#include <config.h>
#include <debug.h>
#include <knobs.h>
#include <midi_io.h>
#include <ui.h>

// Hardware defines

// Knobs
using namespace admux;
Pinset addr(D13, D12, D11);
Mux vala(Pin(A0, INPUT, PinType::Analog), addr);
Mux valb(Pin(A1, INPUT, PinType::Analog), addr);
Mux btn(Pin(D10, INPUT_PULLUP, PinType::Digital), addr);
Knobs knobs(&vala, &valb, &btn);

// LEDs
const uint8_t LedPin = 1;
// https://github.com/Makuna/NeoPixelBus/discussions/878 - RP2350 - support for a 3rd PIO instance
//NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> leds(numParams, LedPin);
NeoPixelBus<NeoRgbFeature, Rp2040x4Pio1Ws2812xMethod> leds(numParams, LedPin); // note: modern WS2812 with letter like WS2812b
// Bitwig colors
HslColor bitwigScheme[] = {
    HslColor(RgbColor(0xf4, 0x1b, 0x3e)), // red
    HslColor(RgbColor(0xff, 0x7f, 0x17)), // orange
    HslColor(RgbColor(0xfc, 0xeb, 0x23)), // yellow
    HslColor(RgbColor(0x5b, 0xc5, 0x15)), // lime
    HslColor(RgbColor(0x65, 0xce, 0x92)), // turquoise
    HslColor(RgbColor(0x5c, 0x8e, 0xee)), // light blue
    HslColor(RgbColor(0xc3, 0x6e, 0xff)), // purple
    HslColor(RgbColor(0xff, 0x54, 0xb0)), // pink
};

// OLEDs
// GPIO18 : SCK Serial Clock)
// GPIO19 : SDA (Master Out, Slave In = Serial Data Output)
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 22, /* reset=*/ 16);
UI ui(&oled1, &oled2);

// USB MIDI object
MidiIO mio;

// Debugging (disable by passing a nullptr)
Debug dbg(&Serial2);

// Callbacks

static void valueCB(unsigned int ix,int value,int delta) {
    dbg.printf("Knob[%u]: Value = %d | Delta = %d\n", ix, value, delta);
    ui.setValue(ix, value);
    mio.sendValueCC(ix, value);
}

static void buttonCB(unsigned int ix, int state) {
    dbg.printf("Button:[%u]; State= %d\n", ix, state);
    mio.sendButtonCC(ix, state*64);
}

void setup() {
    unsigned ts0 = millis();
    leds.Begin(); // call early to reset to black

    dbg.init(5,4);
    dbg.println("Setup start");

    pinMode(LED_BUILTIN, OUTPUT);

    ui.init();

    knobs.setLimits(0,VAL_MAX);
    knobs.setValue(VAL_MAX/2);
    knobs.attachButtonCallback(buttonCB);
    knobs.attachValueCallback(valueCB);
    knobs.begin();

    for (uint16_t i=0; i<numParams; i++) {
        auto hslc = bitwigScheme[i];
        hslc.L = 0.2;  // Brightness from 0.0 to 1.0
        leds.SetPixelColor(i, hslc);
    }
    leds.Show();

    mio.init();

    ui.begin();

    dbg.printf("Setup done: %u ms\n", millis()-ts0);
}

// run midi at full speed
// don't poll the know too quickly though
void loop() {
    unsigned m=millis();
    static unsigned tk = 0;
    if (m - tk > 20) {
        knobs.tick();
        tk = m;
    }

    mio.tick();

    static unsigned tb = 0;
    static bool blink = false;
    if (m - tb > 500)  {
        digitalWrite(LED_BUILTIN, blink);
        blink = !blink;
        tb = m;
    }
}
