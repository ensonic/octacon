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
// using display = U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI;
using display = U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI;
display oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);
//display oled2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 22, /* reset=*/ 16);

// USB MIDI object
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI);


// Function protos

void drawUI(display o);

// Callbacks

static void encodersCB(unsigned int enc,int value,int delta) {
#ifdef USE_LOGING
  Serial.printf("Encoder[%u]: Value = %d | Delta = %d\n",enc,value,delta);
#endif
  switch (enc) {
    case 0: bunt.R = value; break;
    case 1: bunt.G = value; break;
    case 2: bunt.B = value; break;
  }
  drawUI(oled1);
#ifdef USE_MIDI
  MIDI.sendControlChange(9 + enc, value, 1);
#endif
}

static void buttonCB(int enc, int state) {
#ifdef USE_LOGING
    Serial.printf("Button:[%u]; State= %d\n", enc, state);
#endif
}

static void midiControlChangeCB(uint8_t channel, uint8_t controlNumber, uint8_t controlValue) {
}

static void midiSysExCB(byte * array, unsigned size) {
}

void drawUI(display o) {
  // TODO: maybe pass v1...v4 as params
  auto v1 = encoders[0].getValue();
  auto v2 = encoders[1].getValue();
  auto v3 = encoders[2].getValue();
  auto v4 = /*encoders[3].getValue()/2;*/ 50;
  o.clearBuffer();
  o.setDrawColor(1);
  // left column
  o.drawFrame(0, 0, 63, 15);
  o.drawBox(0, 1, v1/2, 14);
  o.drawStr(0, 26 ,"Time L");  // 26 = 16 + 8 + 2
  o.drawStr(0, 45 ,"Feedback");
  o.drawFrame(0, 48, 63, 15);
  o.drawBox(0, 49, v2/2, 14);
  // right column
  o.drawFrame(64, 0, 63, 15);
  o.drawBox(64, 1, v3/2, 14);
  o.drawStr(64, 26 ,"Time R");  // 26 = 16 + 8 + 2
  o.drawStr(64, 45 ,"Dry/Wet");
  o.drawFrame(64, 48, 63, 15);
  o.drawBox(64, 49, v4/2, 14);
  // values
  o.setDrawColor(2);
  o.setCursor(1, 2+8);
  o.print(u8x8_u8toa(v1, 3));
  o.setCursor(1,50+8);
  o.print(u8x8_u8toa(v2, 3));
  o.setCursor(65, 2+8);
  o.print(u8x8_u8toa(v3, 3));
  o.setCursor(65,50+8);
  o.print(u8x8_u8toa(v4, 3));
  o.sendBuffer();  // update oled
}

void setup() {
#ifdef USE_LOGING
  Serial.begin(115200);
  while (!Serial); // wait for serial attach
  Serial.println("Setup start");
#endif

  pinMode(LED_BUILTIN, OUTPUT);

  for(unsigned i=0; i<EncoderCount; i++) {
    encoders[i].setLimits(0,127);
    encoders[i].setValue(50);  
    encoders[i].attachButtonCallback([i](int state) { buttonCB(i, 1-state); }); // invert state, as we do input_pullup
  }
  encoders.begin();
  encoders.attachCallback(encodersCB);

  leds.Begin();
  leds.Show();

  oled1.begin();  //  Start OLED 1
  oled1.setContrast(200);  //  Brightness setting from 0 to 255
  oled1.setFont(u8g2_font_spleen6x12_mf); // m=monochrome,f= full charset
  oled1.setFontMode(1); // make transparent
  drawUI(oled1);

#ifdef USE_MIDI
  TinyUSBDevice.setManufacturerDescriptor("Ensonic");
  TinyUSBDevice.setProductDescriptor("Octacon");
  if (!usbMidi.begin()) {
    Serial.println("Starting usbMidi failed");
  }
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  MIDI.setHandleControlChange(midiControlChangeCB);
  MIDI.setHandleSystemExclusive(midiSysExCB);
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
