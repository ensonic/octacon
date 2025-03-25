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

const unsigned numParams = 8;

// Encoders
// TODO: change to numParams
// TODO: if we switch to analog encoders, we need to change SigA/B to 26/27
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
// GPIO19 : SDA (Master Out, Salve In = Serial Data Output)
// using display = U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI;
using display = U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI;
display oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);
//display oled2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 22, /* reset=*/ 16);
String lastLog="";

// USB MIDI object
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI);
enum class SysExCmd : byte { ParamName, PrettyValue };
const uint8_t ControllerBase = 9;
const unsigned maxNameLen = 14;
char paramNames[numParams][maxNameLen+1] = { 0, };
char prettyValues[numParams][maxNameLen+1] = { 0, };

// Function protos

void drawUI(unsigned ix);
void log(String str);

// Callbacks

static void encodersCB(unsigned int enc,int value,int delta) {
#ifdef USE_LOGING
  Serial.printf("Encoder[%u]: Value = %d | Delta = %d\n",enc,value,delta);
#endif
  drawUI(enc);
#ifdef USE_MIDI
  MIDI.sendControlChange(ControllerBase + enc, value, 1);
#endif
}

static void buttonCB(int enc, int state) {
#ifdef USE_LOGING
    Serial.printf("Button:[%u]; State= %d\n", enc, state);
#endif
#ifdef USE_MIDI
  MIDI.sendControlChange(ControllerBase + numParams + enc, state*64, 1);
#endif
}

static void midiControlChangeCB(uint8_t channel, uint8_t number, uint8_t value) {
  auto enc = number - ControllerBase;
  encoders[enc].setValue(value);
  drawUI(enc);
}

static void midiSysExTextValue(byte *data, unsigned size, char text[numParams][maxNameLen+1]) {
  // param-ix, length, data
  if (size < 2) {
    log("sysexcmd too short");
    return;
  }
  if (data[0] >= numParams) {
    log("sysexcmd bad param-ix");
    return;
  }
  unsigned ix = data[0]; 
  unsigned len = (data[1] < maxNameLen) ? data[1] : maxNameLen;
  strncpy(text[ix], (char *)(&data[2]), len);
  text[ix][len] = '\0';
  drawUI(ix);
}

static void midiSysExCB(byte * data, unsigned size) {
  // min size is 'F0 7D` + cmd + 'F7'
  if (size < 4) {
    log("sysex too short");
    return;
  }
  // bad sysex
  if (data[0] != 0xF0 || data[1] != 0x7D || data[size-1] != 0xF7) {
    log("bad sysex");
    return;
  }
  auto cmd = SysExCmd(data[2]);
  data=&data[3]; size -=4;
  switch(cmd) {
    case SysExCmd::ParamName:
      midiSysExTextValue(data, size, paramNames);
      break;
    case SysExCmd::PrettyValue:
      midiSysExTextValue(data, size, prettyValues);
      break;
    default:
      log("sysex: unknown cmd");
      break;
  }
}

void drawUIPage(display o, unsigned ix0, unsigned ix1, unsigned ix2, unsigned ix3) {
  // TODO: if we update from the callback, we can use updateDisplayArea() instead of sendBuffer()
  // https://github.com/olikraus/u8g2/wiki/u8g2reference#updatedisplayarea
  // TODO: if we want to support longer text, maybe clip? or scroll, smaller fonts?
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
  o.drawStr(0, 45, paramNames[ix2]);
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
  o.drawStr(1, 2+8, prettyValues[ix0]);
  o.drawStr(1,50+8, prettyValues[ix2]);
  o.drawStr(65, 2+8, prettyValues[ix1]);
  o.drawStr(65,50+8, prettyValues[ix3]);
  // TODO: see log()
  if (lastLog.isEmpty()) {
    oled1.drawStr(0, 34, lastLog.c_str());
  }
  o.sendBuffer();  // update display
}

void drawUI(unsigned ix) {
  drawUIPage(oled1, 0, 1, 2, 2);
  /* once we have 2 displays
  if (!(ix & 0x2)) {
    drawUIPage(oled1, 0, 1, 4, 5);
  } else {
    drawUIPage(oled2, 2, 3, 6, 7);
  }
  */
}

// TODO: figure a better way (e.g. 2nd usb serial)
void log(String str) {
#ifdef USE_LOGING
  Serial.println(str);
#else
  lastLog = str;
  drawUI(0);
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

  oled1.begin();
  oled1.setContrast(200);  //  Brightness setting from 0 to 255
  oled1.setFont(
    // 6 pixel
    // u8g2_font_spleen5x8_mf // same width as: u8g2_font_helvR08_tf
    // 7 pixel
    u8g2_font_NokiaSmallPlain_tf
    // 8 pixel
    // u8g2_font_helvR08_tf // not monospaced, can fit more text
    // u8g2_font_spleen6x12_mf // m=monospaced,f= full charset
  );
  oled1.setFontMode(1); // make transparent (no bg)
  drawUIPage(oled1, 0, 1, 2, 2);
  //drawUIPage(oled1, 0, 1, 4, 5);
  //drawUIPage(oled2, 2, 3, 6, 7);

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

void loop() {
  encoders.tick();
#ifdef USE_MIDI
  MIDI.read();
#endif

  static unsigned t0 = 0;
  static bool blink = false;
  if (millis() - t0 > 500)  {
    digitalWrite(LED_BUILTIN, blink);
    blink = !blink;
    t0 = millis();
  }
}
