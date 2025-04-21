// led control code

#include <leds.h>

// Bitwig colors
HslColor bitwigScheme[] = {
    HslColor(RgbColor(0xf4, 0x1b, 0x3e)), // red
    HslColor(RgbColor(0xff, 0x7f, 0x17)), // orange
    HslColor(RgbColor(0xfc, 0xeb, 0x23)), // yellow
    HslColor(RgbColor(0x5b, 0xc5, 0x15)), // lime
    // swap last 4, since we soldered them in wrong order
    HslColor(RgbColor(0xff, 0x54, 0xb0)), // pink
    HslColor(RgbColor(0xc3, 0x6e, 0xff)), // purple
    HslColor(RgbColor(0x5c, 0x8e, 0xee)), // light blue
    HslColor(RgbColor(0x65, 0xce, 0x92)), // turquoise
};


Leds::Leds(LedStripType *npb) : npb(npb) {}

void Leds::begin() {
    npb->Begin();
}

void Leds::SetColors(float brightness) {
    for (uint16_t i=0; i<npb->PixelCount(); i++) {
        auto hslc = bitwigScheme[i];
        hslc.L = brightness;
        npb->SetPixelColor(i, hslc);
    }
    npb->Show();
}

// DEBUG: test RGB with the first 3 encoders
//auto c = RgbColor(knobs.getValue(0)>>6,knobs.getValue(1)>>6,knobs.getValue(2)>>6);
//npb->SetPixelColor(i, c);
