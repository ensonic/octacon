#include <Arduino.h>
#include "EncoderTool.h"
#include <NeoPixelBus.h>
#include <U8g2lib.h>

using namespace EncoderTool;

// Hardware defines

// Encoders
// TODO: change to 8
constexpr unsigned EncoderCount = 3;  // number of attached encoders: 

constexpr unsigned EncMux0 = 11;       // address pin 0
constexpr unsigned EncMux1 = 12;       // ...
constexpr unsigned EncMux2 = 13;       // address pin 2
constexpr unsigned EncSigA = 14;       // output pin SIG of multiplexer A
constexpr unsigned EncSigB = 15;       // output pin SIG of multiplexer B
constexpr unsigned EncSigBtn = 10;     // output pin SIG of multiplexer C
EncPlex4051 encoders(EncoderCount, EncMux0, EncMux1, EncMux2, EncSigA, EncSigB, EncSigBtn);

// LEDs
const uint16_t LedCount = 8;
const uint8_t LedPin = 1;
// https://github.com/Makuna/NeoPixelBus/blob/master/examples/RP2040/NeoPixel_RP2040_PioX4/NeoPixel_RP2040_PioX4.ino
// https://github.com/Makuna/NeoPixelBus/discussions/878 - RP2350 - support for a 3rd PIO instance
//NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> leds(LedCount, LedPin);
NeoPixelBus<NeoGrbFeature, Rp2040x4Pio1Ws2812xMethod> leds(LedCount, LedPin); // note: modern WS2812 with letter like WS2812b
RgbColor black = RgbColor(0);
RgbColor bunt = RgbColor(50);

// OLEDs
// GPIO18 : SCK Serial Clock)
// GPIO19 : SDA (Master Out, Salve In = Serial Data Output)
//U8G2_SSD1327_ZJY_128X128_F_4W_HW_SPI oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);
//U8G2_SSD1327_ZJY_128X128_F_4W_HW_SPI oled2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 22, /* reset=*/ 16);
U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);
U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI oled2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 22, /* reset=*/ 16);


static void encodersCB(unsigned int enc,int value,int delta) {
  Serial.printf("Encoder[%u]: Value = %d | Delta = %d\n",enc,value,delta);
  switch (enc) {
    case 0: bunt.R = value; break;
    case 1: bunt.G = value; break;
    case 2: bunt.B = value; break;
  }
}

static void buttonCB(int enc, int state) {
    Serial.printf("Button:[%u]; State= %d\n", enc, state);
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial attach
  Serial.println("Setup start");

  pinMode(LED_BUILTIN, OUTPUT);

  for(unsigned i=0; i<EncoderCount; i++) {
    encoders[i].setLimits(0,255);
    encoders[i].setValue(50);  
    encoders[i].attachButtonCallback([i](int state) { buttonCB(i, 1-state); }); // invert state, as we do input_pullup
  }
  encoders.begin();
  encoders.attachCallback(encodersCB);

  leds.Begin();
  leds.Show();

  Serial.println("Setup done");
}

static uint16_t iter = 0;
static unsigned t0 = 0;
static bool blink = false;

void loop() {
  encoders.tick();

  if (millis() - t0 > 500)  {
    digitalWrite(LED_BUILTIN, blink);
    blink = !blink;

    for(uint16_t i=0; i<LedCount; i++) {
      if (iter == i) {
        //Serial.printf("%u\n", i);
        leds.SetPixelColor(i, bunt);
      } else {
        leds.SetPixelColor(i, black);
      }
    }
    iter = (iter + 1) % LedCount;
    leds.Show();

    t0 = millis();
  }
}
