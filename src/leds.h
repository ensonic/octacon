// led control code

#ifndef SRC_LEDSS_H
#define SRC_LEDSS_H

#include <NeoPixelBus.h>

// https://github.com/Makuna/NeoPixelBus/discussions/878 - RP2350 - support for a 3rd PIO instance
//NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> leds(numParams, LedPin);
using LedStripType = NeoPixelBus<NeoRgbFeature, Rp2040x4Pio1Ws2812xMethod>;

class Leds {
    public:
      Leds(LedStripType *npb);

      void begin();
      // brightness from 0.0 to 1.0
      void SetColors(float brightness);

    private:
        LedStripType *npb;
};

#endif // SRC_LEDS_H