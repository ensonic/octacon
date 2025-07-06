package de.sonicpulse;

import com.bitwig.extension.api.util.midi.ShortMidiMessage;

public class Mode {
    static final int NumControls = 8;
    static final int CC_MSB_ValueBase = 9;
    static final int CC_LSB_ValueBase = 9 + 32;
    static final int CC_ButtonBase = 9 + 8;

    protected int[] values = new int[NumControls];
    protected String[][] displayValues = new String[NumControls][2];
    protected String[] names = new String[NumControls];
    protected boolean active;
    protected OctaconExtension ext;

    public Mode(OctaconExtension ext) {
        this.ext = ext;
        for (int i = 0; i < NumControls; i++) {
            values[i] = 0;
            displayValues[i][0] = "";
            displayValues[i][1] = "";
            names[i] = "";
        }
    }

    void activate() {
        active = true;
        for (int i = 0; i < NumControls; i++) {
            sendParamValue(i, values[i]);
            sendParamDisplayValue(i, displayValues[i][0]);
            sendParamName(i, names[i]); 
        }
    }

    void deactivate() {
        active = false;
    }

    void onMidi0(ShortMidiMessage msg) {
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
        Logger.log("base onMidi0 done");
    }

    void handleValue(int ix, int data1, int data2) {
    }

    void handleButton(int ix, int data1, int data2) {
        Logger.log("base.button[%d]=%d", ix, data2);
        if (data2 > 0) {
            ext.changeMode();
        }
    }

    void handlePrettyValues() {
        for (int ix = 0; ix < NumControls; ix++) {
            String[] dv = displayValues[ix];
            if (dv[0] == dv[1]) {
                continue;
            }
            dv[1] = dv[0];
            Logger.log("refresh [%d]=%s", ix, dv[1]);
            String hexValue = ext.toHexString(dv[1].replaceAll("[^\\x00-\\x7F]", "").trim());
            ext.sendMidiSysEx(String.format("01 %02x %02x %s", ix, dv[1].length(), hexValue));
        }
    }

    void sendParamValue(int ix, int value) {
        if (active) {
            ext.sendMidiCC(
                new ShortMidiMessage(ShortMidiMessage.CONTROL_CHANGE, CC_MSB_ValueBase + ix, value >> 7),
                new ShortMidiMessage(ShortMidiMessage.CONTROL_CHANGE, CC_LSB_ValueBase + ix, value & 0x127));

        }
    }

    void sendParamName(int ix, String value) {
        if (active) {
            String hexValue = ext.toHexString(value.replaceAll("[^\\x00-\\x7F]", "").trim());
            ext.sendMidiSysEx(String.format("00 %02x %02x %s", ix, value.length(), hexValue));
        }
    }

    void sendParamDisplayValue(int ix, String value) {
        // this will trigger the change detection in the deferred sending
        // we need to use a string that we would otherwise not have
        displayValues[ix][1] = "\n";
    }
}
