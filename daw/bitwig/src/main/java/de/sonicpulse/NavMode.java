package de.sonicpulse;

import com.bitwig.extension.controller.api.ControllerHost;

public class NavMode extends Mode {
    public NavMode(OctaconExtension ext, ControllerHost host) {
        super(ext);
        // TODO: are the more axes?
        names[0] = "Track";
        names[4] = "Device";
        names[7] = "Page";

        displayValues[0][0] = "Unknown";
        displayValues[4][0] = "Unknown";
        displayValues[7][0] = "Unknown";
    }


    @Override
    public void handleValue(int ix, int data1, int data2) {
        Logger.log("nav.knob[%d]=%f", ix, ((float) values[ix]) / 16384.0);
        // TODO: handle navigation
    }

    public void updateInfoString() {
        if (!active) {
            return;
        }
        sendInfoString("");
    }
}
