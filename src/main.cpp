#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <EncoderTool.h>
#include <MIDI.h>
#include <NeoPixelBus.h>
#include <U8g2lib.h>

using namespace EncoderTool;

// Conditionals
#define USE_MIDI
//#define USE_LOGGING

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
//U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);
//U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI oled2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 22, /* reset=*/ 16);
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);


// USB MIDI object
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI);

static void encodersCB(unsigned int enc,int value,int delta) {
#ifdef USE_LOGING
  Serial.printf("Encoder[%u]: Value = %d | Delta = %d\n",enc,value,delta);
#endif
  switch (enc) {
    case 0: bunt.R = value; break;
    case 1: bunt.G = value; break;
    case 2: bunt.B = value; break;
  }
#ifdef USE_MIDI
  MIDI.sendControlChange(9 + enc, value, 1);
#endif
}

static void buttonCB(int enc, int state) {
#ifdef USE_LOGING
    Serial.printf("Button:[%u]; State= %d\n", enc, state);
#endif
}

void setup() {
#ifdef USE_LOGING
  Serial.begin(115200);
  while (!Serial); // wait for serial attach
  Serial.println("Setup start");
#endif

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

  oled1.begin();  //  Start OLED 1
  oled1.setContrast(200);  //  Brightness setting from 0 to 255
  oled1.setFont(u8g2_font_spleen6x12_mf);
  //oled1.setFont(u8g2_font_t0_11_tf);
  oled1.clearBuffer();
  oled1.drawFrame(0, 0, 127, 15);
  oled1.drawBox(1, 1, 50, 14);
  oled1.drawStr(0, 26 ,"Delay Time");  // 25 = 16 + 8 + 2
  oled1.drawStr(0, 45 ,"Feedback");
  oled1.drawFrame(0, 48, 127, 15);
  oled1.drawBox(1, 49, 20, 14);
  oled1.sendBuffer();  // Send to OLED 1

#ifdef USE_MIDI
  TinyUSBDevice.setManufacturerDescriptor("Ensonic");
  TinyUSBDevice.setProductDescriptor("Octacon");
  if (!usbMidi.begin()) {
    Serial.println("Starting usbMidi failed");
  }
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  while( !TinyUSBDevice.mounted() ) delay(1);
#endif
#ifdef USE_LOGING
  Serial.println("Setup done");
#endif
}

static uint16_t iter = 0;
static unsigned t0 = 0;
static bool blink = false;

void loop() {
  encoders.tick();
#ifdef USE_MIDI
  MIDI.read();
#endif

  if (millis() - t0 > 500)  {
    digitalWrite(LED_BUILTIN, blink);
    blink = !blink;

    for(uint16_t i=0; i<LedCount; i++) {
      if (iter == i) {
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
