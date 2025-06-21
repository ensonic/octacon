// led control code

#include <leds.h>

// Bitwig colors
HslColor bitwigColors[] = {
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

HslColor grayscale[] = {
    HslColor(RgbColor(0xff, 0xff, 0xff)),
    HslColor(RgbColor(0xff, 0xff, 0xff)),
    HslColor(RgbColor(0xff, 0xff, 0xff)),
    HslColor(RgbColor(0xff, 0xff, 0xff)),
    HslColor(RgbColor(0xff, 0xff, 0xff)),
    HslColor(RgbColor(0xff, 0xff, 0xff)),
    HslColor(RgbColor(0xff, 0xff, 0xff)),
    HslColor(RgbColor(0xff, 0xff, 0xff)),
};

HslColor *patterns[] {
    bitwigColors,
    grayscale,
};
static unsigned num_patterns = (sizeof(patterns)/sizeof(*patterns));

Leds::Leds(LedStripType *npb1, LedStripType *npb2) : npb1(npb1), npb2(npb2) {}

void Leds::begin() {
    npb1->Begin();
    if (npb2 == nullptr) return;
    npb2->Begin();
}

void Leds::setBrightness(float brightness) {
    this->brightness = brightness;
    setColors();
 }

void Leds::setPattern(unsigned pattern) {
    if (pattern >= num_patterns) {
        pattern = 0;
    }
    this->pattern = pattern;
    setColors();
}

void Leds::setColors() {
    uint16_t bwci = 0;
    HslColor *colors = patterns[pattern];

    for (uint16_t i=0; i<npb1->PixelCount(); i++) {
        auto hslc = colors[bwci];
        hslc.L = brightness;
        npb1->SetPixelColor(i, hslc);
        bwci++;
    }
    npb1->Show();
    if (npb2 == nullptr) return;
    for (uint16_t i=0; i<npb2->PixelCount(); i++) {
        auto hslc = colors[bwci];
        hslc.L = brightness;
        npb2->SetPixelColor(i, hslc);
        bwci++;
    }
    npb2->Show();
}

// DEBUG: test RGB with the first 3 encoders
//auto c = RgbColor(knobs.getValue(0)>>6,knobs.getValue(1)>>6,knobs.getValue(2)>>6);
//npb1->SetPixelColor(i, c);
