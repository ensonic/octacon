package de.sonicpulse;

import com.bitwig.extension.controller.api.ControllerHost;
import com.bitwig.extension.controller.api.MidiOut;

import java.util.ArrayDeque;
import java.util.NoSuchElementException;

import com.bitwig.extension.api.util.midi.ShortMidiMessage;
import com.bitwig.extension.callback.ShortMidiMessageReceivedCallback;
import com.bitwig.extension.controller.ControllerExtension;

/*
 * build & install
 *   cd daw/bitwig
 *   mvn install
 *   cp target/Octacon.bwextension ~/Bitwig\ Studio/Extensions/
 * 
 * todo: port more from js code
 * - info text
 *   - maybe move completely to ctrlmode, in navmode we'll watch similar thing,
 *     but use it for different purposes
 */
public class OctaconExtension extends ControllerExtension {
    static final String SYSEX_BEGIN = "F0 7D ";
    static final String SYSEX_END = " F7";
    // Daw-Mode (on/off)
    static final String SYSEX_HELLO = SYSEX_BEGIN + "02 01" + SYSEX_END;
    static final String SYSEX_BYE = SYSEX_BEGIN + "02 00" + SYSEX_END;
    static final int SYSEX_RATE = 100;

    private Mode[] modes = new Mode[2];
    private int mode = 0;

    private ControllerHost host;
    private MidiOut outPort;

    private ArrayDeque<ShortMidiMessage> midiCC = new ArrayDeque<ShortMidiMessage>();
    private ArrayDeque<String> midiSysEx = new ArrayDeque<String>();

    private Runnable queueDisplayValueChanges = new QueueDisplayValueChanges();

    protected OctaconExtension(final OctaconExtensionDefinition definition, final ControllerHost host) {
        super(definition, host);
    }

    @Override
    public void init() {
        host = getHost();
        Logger.init(host);

        outPort = host.getMidiOutPort(0);
        outPort.sendSysex(SYSEX_HELLO);

        modes[0] = new CtrlMode(this, host);
        modes[1] = new NavMode(this, host);

        // start receiving data
        host.getMidiInPort(0).setMidiCallback((ShortMidiMessageReceivedCallback) msg -> onMidi0(msg));

        // rate limit sending the pretty values, when using automation this can
        // otherwise overload the controller
        host.scheduleTask(queueDisplayValueChanges, SYSEX_RATE);
        modes[mode].activate();

        Logger.log("Octacon initialized");
    }

    @Override
    public void exit() {
        outPort.sendSysex(SYSEX_BYE);
    }

    @Override
    public void flush() {
        try {
            while (true) {
                ShortMidiMessage m = midiCC.remove();
                outPort.sendMidi(m.getStatusByte(), m.getData1(), m.getData2());
            }
        } catch (NoSuchElementException e) {
        } // queue empty
        try {
            while (true) {
                String m = midiSysEx.remove();
                outPort.sendSysex(SYSEX_BEGIN + m + SYSEX_END);
            }
        } catch (NoSuchElementException e) {
        } // queue empty
    }

    public void changeMode() {
        modes[mode].deactivate();
        mode = 1 - mode;
        // toggle led-color on device
        outPort.sendSysex(SYSEX_BEGIN + "03 " + String.format("%02x ", mode) + SYSEX_END);
        modes[mode].activate();
    }

    public void sendMidiCC(ShortMidiMessage m1, ShortMidiMessage m2) {
        midiCC.add(m1);
        midiCC.add(m2);
    }

    public void sendMidiSysEx(String data) {
        midiSysEx.add(data);
    }

    public String toHexString(String s) {
        String h = "";
        for (int i = 0; i < s.length(); i++) {
            h += String.format("%02x ", (byte) s.charAt(i));
        }
        return h.trim();
    }

    private void onMidi0(ShortMidiMessage msg) {
        if (!msg.isControlChange()) {
            return;
        }
        modes[mode].onMidi0(msg);
    }

    private class QueueDisplayValueChanges implements Runnable {
        @Override
        public void run() {
            modes[mode].handlePrettyValues();
            host.scheduleTask(queueDisplayValueChanges, SYSEX_RATE);
        }
    }
}
