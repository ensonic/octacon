// led control code

#ifndef SRC_LEDS_H
#define SRC_LEDS_H

#include <NeoPixelBus.h>

#include <config.h>

// https://github.com/Makuna/NeoPixelBus/discussions/878 - RP2350 - support for a 3rd PIO instance
//NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> leds(numParams, LedPin);
#if HW_VER == 1
using LedStripType = NeoPixelBus<NeoRgbFeature, Rp2040x4Pio1Ws2812xMethod>;
#elif HW_VER == 2
using LedStripType = NeoPixelBus<NeoGrbFeature, Rp2040x4Pio1Ws2812xMethod>;
#endif

class Leds {
    public:
      Leds(LedStripType *npb1, LedStripType *npb2=nullptr);

      void begin();
      // brightness from 0.0 to 1.0
      void setBrightness(float brightness);
      // select one of the built-in (color) patterns
      void setPattern(unsigned pattern);

    private:
        LedStripType *npb1, *npb2;
        unsigned pattern = 0;
        float brightness = 0.05;

        void setColors();
};

#endif // SRC_LEDS_H