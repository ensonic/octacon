// midi handling code

#ifndef SRC_MIDI_IO_H
#define SRC_MIDI_IO_H

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <config.h>

// 14-bit MIDI uses two midi CC for one control. The standard is to have CC 0-31
// send the MSB (Most Significant Bit) and CC 32-63 send the LSB (Least
// Significant Bit).
const uint8_t CC_MSB_ValueBase = 9;
const uint8_t CC_LSB_ValueBase = 9 + 32;
const uint8_t CC_ButtonBase = 9 + 8;

enum class SysExCmd : byte {
    ParamName,      // receive parameter names
    PrettyValue,    // receive human readable parameter values
    DawSync,        // daw attached?
    LedPattern,     // chose on of teh supported led (color) patterns
};

class MidiIO {
public:
    MidiIO(void);

    void init(void);
    void tick(void);

    void sendValueCC(uint8_t cc, int v);
    void sendButtonCC(uint8_t cc, int v);

private:
    using MidiTransport = MIDI_NAMESPACE::SerialMIDI<Adafruit_USBD_MIDI>;
    using MidiInterface = MIDI_NAMESPACE::MidiInterface<MidiTransport>;

    MidiTransport midiTransport;
    MidiInterface midiInterface;
};

#endif // SRC_MIDI_IO_H