// midi handling code

#include <EncoderTool.h>

#include <debug.h>
#include <midi_io.h>
#include <ui.h>

extern Debug dbg;
extern UI ui;
extern EncoderTool::EncPlex4051 encoders;

Adafruit_USBD_MIDI usbMidi;

//midi::MidiInterface<midi::SerialMIDI<Adafruit_USBD_MIDI>> MIDI;

static void midiControlChangeCB(uint8_t channel, uint8_t number, uint8_t value) {
    auto enc = number - ControllerBase;
    encoders[enc].setValue(value);
    ui.setValue(enc, value);
}

static void midiSysExParamName(byte *data, unsigned size) {
    // param-ix, length, data
    if (size < 2) {
        dbg.printf("sysexcmd-%u too short\n", SysExCmd::ParamName);
        return;
    }
    ui.setName(data[0], (char *)(&data[2]), data[1]);
}

static void midiSysExPrettyValue(byte *data, unsigned size) {
    // param-ix, length, data
    if (size < 2) {
        dbg.printf("sysexcmd-%u too short\n", SysExCmd::PrettyValue);
        return;
    }
     ui.setPrettyValue(data[0], (char *)(&data[2]), data[1]);
}

static void midiSysExDawSync(byte *data, unsigned size) {
    // on/off
    if (size < 1) {
        dbg.printf("sysexcmd-%u too short\n", SysExCmd::DawSync);
        return;
    }
     ui.enableExtInfo(data[0]>0);
}

static void midiSysExCB(byte * data, unsigned size) {
    // min size is 'F0 7D` + cmd + 'F7'
    if (size < 4) {
        dbg.println("sysex too short");
        return;
    }
    // bad sysex
    if (data[0] != 0xF0 || data[1] != 0x7D || data[size-1] != 0xF7) {
        dbg.println("bad sysex");
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
        case SysExCmd::DawSync:
            midiSysExDawSync(data, size);
            break;
        default:
            dbg.println("sysex: unknown cmd");
            break;
    }
}

MidiIO::MidiIO(void) : midiTransport(usbMidi), midiInterface((MidiTransport&)midiTransport) {}

void MidiIO::init(void) {
    TinyUSBDevice.setManufacturerDescriptor("Ensonic");
    TinyUSBDevice.setProductDescriptor("Octacon");
    if (!usbMidi.begin()) {
        dbg.println("Starting usbMidi failed");
    }
    midiInterface.begin(MIDI_CHANNEL_OMNI);
    midiInterface.turnThruOff();
    midiInterface.setHandleControlChange(midiControlChangeCB);
    midiInterface.setHandleSystemExclusive(midiSysExCB);
    while( !TinyUSBDevice.mounted() ) delay(1);
}

void MidiIO::tick(void) {
    midiInterface.read();
}

void MidiIO::sendCC(uint8_t cc, uint8_t v) {
    midiInterface.sendControlChange(ControllerBase + cc, v, 1);
}