package de.sonicpulse;

import com.bitwig.extension.api.util.midi.ShortMidiMessage;

public class Mode {
    static final int NumControls = 8;
    static final int CC_MSB_ValueBase = 9;
    static final int CC_LSB_ValueBase = 9 + 32;
    static final int CC_ButtonBase = 9 + 8;
    // flag constants
    static final int Flag_HasAutomation = 1 << 0;

    protected int ledPattern = 0;
    protected int[] values = new int[NumControls];
    protected int[] ticks = new int[NumControls];
    protected int[] flags = new int[NumControls]; // could use java.util.BitSet, but seems overkill
    protected String[][] displayValues = new String[NumControls][2];
    protected String[] names = new String[NumControls];
    protected boolean active;
    protected OctaconExtension ext;

    public Mode(OctaconExtension ext) {
        this.ext = ext;
        for (int i = 0; i < NumControls; i++) {
            values[i] = 0;
            ticks[i] = 0;
            flags[i] = 0;
            displayValues[i][0] = "";
            displayValues[i][1] = "";
            names[i] = "";
        }
    }

    public void activate() {
        active = true;
        sendLedPattern(ledPattern);
        for (int i = 0; i < NumControls; i++) {
            sendParamValue(i, values[i]);
            sendParamDisplayValue(i, displayValues[i][0]);
            sendParamName(i, names[i]); 
            sendParamTicks(i, ticks[i]);
            sendParamFlags(i, flags[i]);
        }
        updateInfoString();
    }

    public void deactivate() {
        active = false;
    }

    public void onMidi0(ShortMidiMessage msg) {
        if (!active) return;
        int ix = -1;
        int data1 = msg.getData1();
        int data2 = msg.getData2();
        if (data1 >= CC_MSB_ValueBase && data1 < CC_MSB_ValueBase + NumControls) {
            ix = data1 - CC_MSB_ValueBase;
            values[ix] = data2 << 7;
            handleValue(ix, data1, data2);
        }
        if (data1 >= CC_LSB_ValueBase && data1 < CC_LSB_ValueBase + NumControls) {
            ix = data1 - CC_LSB_ValueBase;
            values[ix] += data2;
            handleValue(ix, data1, data2);
        }
        if (data1 >= CC_ButtonBase && data1 < CC_ButtonBase + NumControls) {
            ix = data1 - CC_ButtonBase;
            handleButton(ix, data1, data2);
        }
    }

    public void handleValue(int ix, int data1, int data2) {
    }

    public void handleButton(int ix, int data1, int data2) {
        // Logger.log("base.button[%d]=%d", ix, data2);
        if (data2 > 0) {
            ext.changeMode();
        }
    }

    public void updateInfoString() {
    }

    public void handlePrettyValues() {
        if (!active) return;
        for (int ix = 0; ix < NumControls; ix++) {
            String[] dv = displayValues[ix];
            if (dv[0] == dv[1]) {
                continue;
            }
            dv[1] = dv[0];
            String hexValue = ext.toHexString(dv[1].replaceAll("[^\\x00-\\x7F]", "").trim());
            ext.sendMidiSysEx(String.format("01 %02x %02x %s", ix, dv[1].length(), hexValue));
        }
    }

    protected void sendParamValue(int ix, int value) {
        if (!active) return;
        ext.sendMidiCC(
            new ShortMidiMessage(ShortMidiMessage.CONTROL_CHANGE, CC_MSB_ValueBase + ix, value >> 7),
            new ShortMidiMessage(ShortMidiMessage.CONTROL_CHANGE, CC_LSB_ValueBase + ix, value & 127));
    }

    protected void sendParamName(int ix, String value) {
        if (!active) return;
        String hexValue = ext.toHexString(value.replaceAll("[^\\x00-\\x7F]", "").trim());
        ext.sendMidiSysEx(String.format("00 %02x %02x %s", ix, value.length(), hexValue));
    }

    protected void sendParamDisplayValue(int ix, String value) {
        // this will trigger the change detection in the deferred sending
        // we need to use a string that we would otherwise not have
        displayValues[ix][1] = "\n";
    }

    protected void sendParamTicks(int ix, int value) {
        if (!active) return;
        ext.sendMidiSysEx(String.format("05 %02x %02x", ix, value));
    }

    protected void sendParamFlags(int ix, int value) {
        if (!active) return;
        ext.sendMidiSysEx(String.format("06 %02x %02x", ix, value));
    }

    protected void sendLedPattern(int mode) {
        // toggle led-color on device
        ext.sendMidiSysEx(String.format("03 %02x ", mode));
    }

    protected void sendInfoString(String value) {
        if (!active) return;
        String hexValue = ext.toHexString(value.replaceAll("[^\\x00-\\x7F]", "").trim());
        ext.sendMidiSysEx(String.format("04 %02x %s", value.length(), hexValue));
    }
}
