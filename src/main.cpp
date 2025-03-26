#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <EncoderTool.h>
#include <MIDI.h>
#include <NeoPixelBus.h>
#include <U8g2lib.h>

#include <ui.h>

using namespace EncoderTool;

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
// TODO: change to U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI;
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI oled1(U8G2_R0, /* cs=*/ 21, /* dc=*/ 22, /* reset=*/ 20);
//U8G2_SSD1309_128X64_NONAME2_F_2ND_4W_HW_SPI oled2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 22, /* reset=*/ 16);
UI ui(&oled1);

// USB MIDI object
Adafruit_USBD_MIDI usbMidi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMidi, MIDI);
// TODO: maybe add daw-connect/disconnect cmd,
// daw-disconnect: reset names/values:  ui then shows cc-names on screen and midi values
enum class SysExCmd : byte { ParamName, PrettyValue };
const uint8_t ControllerBase = 9;

// Function protos

// Callbacks

static void encodersCB(unsigned int enc,int value,int delta) {
  Serial2.printf("Encoder[%u]: Value = %d | Delta = %d\n",enc,value,delta);
  ui.setValue(enc, value);
  MIDI.sendControlChange(ControllerBase + enc, value, 1);
}

static void buttonCB(int enc, int state) {
  Serial2.printf("Button:[%u]; State= %d\n", enc, state);
  MIDI.sendControlChange(ControllerBase + numParams + enc, state*64, 1);
}

static void midiControlChangeCB(uint8_t channel, uint8_t number, uint8_t value) {
  auto enc = number - ControllerBase;
  encoders[enc].setValue(value);
  ui.setValue(enc, value);
}

static void midiSysExParamName(byte *data, unsigned size) {
  // param-ix, length, data
  if (size < 2) {
    Serial2.println("sysexcmd too short");
    return;
  }
  if (data[0] >= numParams) {
    Serial2.println("sysexcmd bad param-ix");
    return;
  }
  ui.setName(data[0], (char *)(&data[2]), data[1]);
}

static void midiSysExPrettyValue(byte *data, unsigned size) {
  // param-ix, length, data
  if (size < 2) {
    Serial2.println("sysexcmd too short");
    return;
  }
  if (data[0] >= numParams) {
    Serial2.println("sysexcmd bad param-ix");
    return;
  }
  ui.setPrettyValue(data[0], (char *)(&data[2]), data[1]);
}

static void midiSysExCB(byte * data, unsigned size) {
  // min size is 'F0 7D` + cmd + 'F7'
  if (size < 4) {
    Serial2.println("sysex too short");
    return;
  }
  // bad sysex
  if (data[0] != 0xF0 || data[1] != 0x7D || data[size-1] != 0xF7) {
    Serial2.println("bad sysex");
    return;
  }
  auto cmd = SysExCmd(data[2]);
  data=&data[3]; size -=4;
  switch(cmd) {
    case SysExCmd::ParamName:
      midiSysExParamName(data, size);
      break;
    case SysExCmd::PrettyValue:
      midiSysExPrettyValue(data, size);
      break;
    default:
      Serial2.println("sysex: unknown cmd");
      break;
  }
}

void setup() {
  Serial2.setRX(5);
  Serial2.setTX(4);
  Serial2.begin(115200);
  while (!Serial2); // wait for serial attach
  Serial2.println("Setup start");

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

  ui.init();

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

  Serial2.println("Setup done");
}

void loop() {
  encoders.tick();
  MIDI.read();

  static unsigned t0 = 0;
  static bool blink = false;
  if (millis() - t0 > 500)  {
    digitalWrite(LED_BUILTIN, blink);
    blink = !blink;
    t0 = millis();
  }
}
