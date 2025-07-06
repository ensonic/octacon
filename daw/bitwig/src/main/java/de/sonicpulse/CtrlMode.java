package de.sonicpulse;

import com.bitwig.extension.controller.api.ControllerHost;
import com.bitwig.extension.controller.api.CursorDevice;
import com.bitwig.extension.controller.api.CursorRemoteControlsPage;
import com.bitwig.extension.controller.api.CursorTrack;
import com.bitwig.extension.controller.api.RemoteControl;

public class CtrlMode extends Mode {
    private CursorTrack cursorTrack;
    private CursorDevice cursorDevice;
    private CursorRemoteControlsPage remoteControlCursor;

    public CtrlMode(OctaconExtension ext, ControllerHost host) {
        super(ext);
        // active device that follows UI selection
        cursorTrack = host.createCursorTrack(0, 0);
        cursorDevice = cursorTrack.createCursorDevice();
        remoteControlCursor = cursorDevice.createCursorRemoteControlsPage(NumControls);

        for (int i = 0; i < remoteControlCursor.getParameterCount(); i++) {
            final int ix = i;
            RemoteControl param = remoteControlCursor.getParameter(i);
            param.markInterested();
            param.setIndication(true);
            param.value().addValueObserver(16384, (value) -> onParamValueChange(ix, value));
            param.name().addValueObserver((value) -> onParamNameChange(ix, value));
            param.displayedValue().addValueObserver((value) -> onParamDisplayedValueChange(ix, value));
            /*
             * also send how many ticks we have
             * param.discreteValueCount().addValueObserver(...)
             */
        }
    }

    @Override
    void handleValue(int ix, int data1, int data2) {
        Logger.log("ctrl.knob[%d]=%f", ix, ((float) values[ix]) / 16384.0);
        remoteControlCursor.getParameter(ix).value().set(values[ix] / 16384.0);
    }

    void onParamValueChange(int ix, int value) {
        Logger.log("value[%d]=%d", ix, value);
        sendParamValue(ix, value);
    }

    void onParamNameChange(int ix, String value) {
        Logger.log("name[%d]=%s", ix, value);
        sendParamName(ix, value);
        names[ix] = value;
    }

    void onParamDisplayedValueChange(int ix, String value) {
        Logger.log("prettyValue[%d]=%s", ix, value);
        displayValues[ix][0] = value;
    }
}
