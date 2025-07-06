package de.sonicpulse;

import java.util.HashMap;
import java.util.Map;

import com.bitwig.extension.controller.api.ControllerHost;
import com.bitwig.extension.controller.api.CursorDevice;
import com.bitwig.extension.controller.api.CursorRemoteControlsPage;
import com.bitwig.extension.controller.api.CursorTrack;
import com.bitwig.extension.controller.api.RemoteControl;

public class CtrlMode extends Mode {
    private CursorTrack cursorTrack;
    private CursorDevice cursorDevice;
    private CursorRemoteControlsPage remoteControlCursor;
    private HashMap<String, String> info = new HashMap<>();
    private String[] pageNames = null;
    private int pageIx = -1;

    public CtrlMode(OctaconExtension ext, ControllerHost host) {
        super(ext);
        // active device that follows UI selection
        cursorTrack = host.createCursorTrack(0, 0);
        cursorDevice = cursorTrack.createCursorDevice();
        remoteControlCursor = cursorDevice.createCursorRemoteControlsPage(NumControls);

        for (int i = 0; i < remoteControlCursor.getParameterCount(); i++) {
            final int ix = i;
            RemoteControl param = remoteControlCursor.getParameter(i);
            param.setIndication(true);
            param.value().addValueObserver(16384, (value) -> {
                sendParamValue(ix, value);
            });
            param.name().addValueObserver((value) -> {
                sendParamName(ix, value);
                names[ix] = value;
            });
            param.displayedValue().addValueObserver((value) -> {
                displayValues[ix][0] = value;
            });
            /* TODO: also send how many ticks we have
             * param.discreteValueCount().addValueObserver(...)
             */
        }

        Map.of(
            "track", cursorTrack.name(),
            "device", cursorDevice.name(),
            // TODO: this works for device/preset-pages, but not for module/modulator pages ??
		    // e.g. open Polymer and select "Union" or "Vibrato"
            // Same issue in js, reported to Bitwig in 06/2025
            "page", remoteControlCursor.getName()
        ).forEach((k,v) -> {
            final String key = k;
            v.addValueObserver((value) -> {
                if (value == "" ) {
                    return;
                }
                info.put(key, value);
                updateInfoString();
            });
        });
        // complex workaround to get all page-names (see TODO above)
        remoteControlCursor.pageNames().addValueObserver((value) -> {
            pageNames = value;
            if (pageIx < 0 || pageNames == null || pageNames.length <= pageIx) {
                return;
            }
            info.put("page", pageNames[pageIx]);
            updateInfoString();
        });
        remoteControlCursor.selectedPageIndex().addValueObserver((value) -> {
            pageIx = value;
            if (pageIx < 0 || pageNames == null || pageNames.length <= pageIx) {
                return;
            }
            info.put("page", pageNames[pageIx]);
            updateInfoString();
        });

    }

    @Override
    public void handleValue(int ix, int data1, int data2) {
        remoteControlCursor.getParameter(ix).value().set(values[ix] / 16384.0);
    }

    @Override
    public void updateInfoString() {
        if (!active) {
            return;
        }
        sendInfoString(String.format("%s/%s/%s",
            info.getOrDefault("track", ""),
            info.getOrDefault("device", ""),
            info.getOrDefault("page", "")));
    }
}
