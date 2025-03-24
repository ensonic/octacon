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
// GPIO19 : SDA (Master Out, Salve In = Serial Data Output)
// using display = U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI;
using display = U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI;
display oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);
//display oled2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 22, /* reset=*/ 16);

// USB MIDI object
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI);
const uint8_t ControllerBase = 9;
const int maxNameLen = 16;
char paramNames[8][maxNameLen] = {
  "Time L",
  "Time R",
  "Feedback",
  "Dry/Wet",
  "",
  "",
  "",
  "",
};

// Function protos

void drawUI(display o, int ix0, int ix1, int ix2, int ix3);

// Callbacks

static void encodersCB(unsigned int enc,int value,int delta) {
#ifdef USE_LOGING
  Serial.printf("Encoder[%u]: Value = %d | Delta = %d\n",enc,value,delta);
#endif
  if (!(enc & 0x2)) {
    drawUI(oled1, 0, 1, 2, 2);
    //drawUI(oled1, 0, 1, 4, 5);
  } /*else {
    drawUI(oled2, 2, 3, 6, 7);
  }*/
#ifdef USE_MIDI
  MIDI.sendControlChange(ControllerBase + enc, value, 1);
#endif
}

static void buttonCB(int enc, int state) {
#ifdef USE_LOGING
    Serial.printf("Button:[%u]; State= %d\n", enc, state);
#endif
}

static void midiControlChangeCB(uint8_t channel, uint8_t number, uint8_t value) {
  auto enc = number - ControllerBase;
  encoders[enc].setValue(value);
  if (!(enc & 0x2)) {
    drawUI(oled1, 0, 1, 2, 2);
    //drawUI(oled1, 0, 1, 4, 5);
  } /*else {
    drawUI(oled2, 2, 3, 6, 7);
  }*/
}

static void midiSysExCB(byte * array, unsigned size) {
  // TODO: update param names
}

void drawUI(display o, int ix0, int ix1, int ix2, int ix3) {
  // TODO: if we update from the callback, we can use updateDisplayArea() instead of sendBuffer()
  // https://github.com/olikraus/u8g2/wiki/u8g2reference#updatedisplayarea
  auto v0 = encoders[ix0].getValue();
  auto v1 = encoders[ix1].getValue();
  auto v2 = encoders[ix2].getValue();
  auto v3 = encoders[ix3].getValue();
  o.clearBuffer();
  o.setDrawColor(1);
  // left column
  o.drawFrame(0, 0, 63, 15);
  o.drawBox(0, 1, v0/2, 14);
  o.drawStr(0, 26, paramNames[ix0]);  // 26 = 16 + 8 + 2
  o.drawStr(0, 45, paramNames[ix3]);
  o.drawFrame(0, 48, 63, 15);
  o.drawBox(0, 49, v2/2, 14);
  // right column
  o.drawFrame(64, 0, 63, 15);
  o.drawBox(64, 1, v1/2, 14);
  o.drawStr(64, 26, paramNames[ix1]);  // 26 = 16 + 8 + 2
  o.drawStr(64, 45, paramNames[ix3]);
  o.drawFrame(64, 48, 63, 15);
  o.drawBox(64, 49, v3/2, 14);
  // values
  o.setDrawColor(2);
  o.setCursor(1, 2+8);
  o.print(u8x8_u8toa(v0, 3));
  o.setCursor(1,50+8);
  o.print(u8x8_u8toa(v2, 3));
  o.setCursor(65, 2+8);
  o.print(u8x8_u8toa(v1, 3));
  o.setCursor(65,50+8);
  o.print(u8x8_u8toa(v3, 3));
  o.sendBuffer();  // update display
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
  for (uint16_t i=0; i<LedCount; i++) {
    auto hslc = bitwigScheme[i];
    hslc.L = encoders[i].getValue() / 127.0f;
    leds.SetPixelColor(i, hslc);
  }
  leds.Show();

  oled1.begin();  //  Start OLED 1
  oled1.setContrast(200);  //  Brightness setting from 0 to 255
  oled1.setFont(u8g2_font_spleen6x12_mf); // m=monochrome,f= full charset
  oled1.setFontMode(1); // make transparent
  drawUI(oled1, 0, 1, 2, 2);
  //drawUI(oled1, 0, 1, 4, 5);
  //drawUI(oled2, 2, 3, 6, 7);

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

    iter = (iter + 1) % LedCount;
    leds.Show();

    t0 = millis();
  }
}
