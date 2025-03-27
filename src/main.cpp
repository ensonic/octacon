#include <Arduino.h>
#include <EncoderTool.h>
#include <NeoPixelBus.h>
#include <U8g2lib.h>

#include <debug.h>
#include <midi_io.h>
#include <ui.h>

using namespace EncoderTool;

// Hardware defines

const unsigned numParams = 8;

// Encoders
// TODO: change to numParams
// TODO: if we switch to analog encoders:
// - change SigA/B from 14/15 to 26/27
// - change GND for Multiplexers to use ADC GND right next to it
constexpr unsigned EncoderCount = 3;  // number of attached encoders
constexpr unsigned EncMux0 = 11;       // address pin 0
constexpr unsigned EncMux1 = 12;       // ...
constexpr unsigned EncMux2 = 13;       // address pin 2
constexpr unsigned EncSigA = 14;       // output pin SIG of multiplexer A
constexpr unsigned EncSigB = 15;       // output pin SIG of multiplexer B
constexpr unsigned EncSigBtn = 10;     // output pin SIG of multiplexer C
EncPlex4051 encoders(EncoderCount, EncMux0, EncMux1, EncMux2, EncSigA, EncSigB, EncSigBtn);

// LEDs
const uint16_t LedCount = numParams;
const uint8_t LedPin = 1;
// https://github.com/Makuna/NeoPixelBus/discussions/878 - RP2350 - support for a 3rd PIO instance
//NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> leds(LedCount, LedPin);
NeoPixelBus<NeoGrbFeature, Rp2040x4Pio1Ws2812xMethod> leds(LedCount, LedPin); // note: modern WS2812 with letter like WS2812b
// Bitwig colors
HslColor bitwigScheme[] = {
    HslColor(RgbColor(0xf7, 0x1b, 0x3e)), // red
    HslColor(RgbColor(0xff, 0x7f, 0x17)), // orange
    HslColor(RgbColor(0xfc, 0xeb, 0x23)), // yellow
    HslColor(RgbColor(0x5b, 0xc5, 0x15)), // lime
    HslColor(RgbColor(0x65, 0xce, 0x92)), // turquoise
    HslColor(RgbColor(0x5c, 0xa8, 0xee)), // light blue
    HslColor(RgbColor(0xc3, 0x6e, 0xff)), // purple
    HslColor(RgbColor(0xff, 0x54, 0xb0)), // pink
};

// OLEDs
// GPIO18 : SCK Serial Clock)
// GPIO19 : SDA (Master Out, Slave In = Serial Data Output)
// TODO: change to U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI;
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);
//U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI oled2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 22, /* reset=*/ 16);
UI ui(&oled1);

// USB MIDI object
MidiIO mio;

// Debugging (disable by passing a nullptr)
Debug dbg(&Serial2);

// Function protos

// Callbacks

static void encodersCB(unsigned int enc,int value,int delta) {
    dbg.printf("Encoder[%u]: Value = %d | Delta = %d\n",enc,value,delta);
    ui.setValue(enc, value);
    mio.sendCC(enc, value);
}

static void buttonCB(int enc, int state) {
    dbg.printf("Button:[%u]; State= %d\n", enc, state);
    mio.sendCC(numParams + enc, state*64);
}

void setup() {
    unsigned ts0 = millis();
    dbg.init(5,4);
    dbg.println("Setup start");

    pinMode(LED_BUILTIN, OUTPUT);

    ui.init();

    for(unsigned i=0; i<EncoderCount; i++) {
        encoders[i].setLimits(0,127);
        encoders[i].setValue(50);
        encoders[i].attachButtonCallback([i](int state) { buttonCB(i, 1-state); }); // invert state, as we do input_pullup
    }
    encoders.begin();
    encoders.attachCallback(encodersCB);

    leds.Begin();
    for (uint16_t i=0; i<LedCount; i++) {
        auto hslc = bitwigScheme[i];
        hslc.L = 0.3;  // Brightness from 0.0 to 1.0
        leds.SetPixelColor(i, hslc);
    }
    leds.Show();

    mio.init();

    ui.begin();

    dbg.printf("Setup done: %u ms\n", millis()-ts0);
}

void loop() {
    encoders.tick();
    mio.tick();

    static unsigned t0 = 0;
    static bool blink = false;
    if (millis() - t0 > 500)  {
        digitalWrite(LED_BUILTIN, blink);
        blink = !blink;
        t0 = millis();
    }
}
