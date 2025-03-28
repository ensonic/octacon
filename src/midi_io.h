// midi handling code

#ifndef SRC_MIDI_IO_H
#define SRC_MIDI_IO_H

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

const uint8_t ControllerBase = 9;

enum class SysExCmd : byte {
    ParamName,      // receive parameter names
    PrettyValue,    // receive human readable parameter values
    DawSync,        // daw attached?
};

class MidiIO {
public:
    MidiIO(void);

    void init(void);
    void tick(void);

    void sendCC(uint8_t cc, uint8_t v);

private:
    using MidiTransport = MIDI_NAMESPACE::SerialMIDI<Adafruit_USBD_MIDI>;
    using MidiInterface = MIDI_NAMESPACE::MidiInterface<MidiTransport>;

    MidiTransport midiTransport;
    MidiInterface midiInterface;
};

#endif // SRC_MIDI_IO_H