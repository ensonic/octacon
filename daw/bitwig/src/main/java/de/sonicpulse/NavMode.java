package de.sonicpulse;

import com.bitwig.extension.controller.api.ControllerHost;
import com.bitwig.extension.controller.api.CursorDevice;
import com.bitwig.extension.controller.api.CursorRemoteControlsPage;
import com.bitwig.extension.controller.api.CursorTrack;

public class NavMode extends Mode {
    private CursorTrack cursorTrack;
    private CursorDevice cursorDevice;
    private CursorRemoteControlsPage remoteControlCursor;
    private String[] pageNames = null;
    private int pageIx = -1;

    private enum NavParam {
        Track(0, "Track"),
        Device(4, "Device"),
        Page(7,"Page");

        public final int ix;
        public final String label;

        private NavParam(int ix, String label) {
            this.ix = ix;
            this.label = label;
        }

        public static NavParam valueOfIx(int ix) {
            for (NavParam e : values()) {
                if (e.ix == ix) {
                    return e;
                }
            }
            return null;
        }
    }

    public NavMode(OctaconExtension ext, ControllerHost host) {
        super(ext);
        ledPattern = 1;
        // TODO: are the more axes?
        names[NavParam.Track.ix] = NavParam.Track.label;
        names[NavParam.Device.ix] = NavParam.Device.label;
        names[NavParam.Page.ix] = NavParam.Page.label;

        displayValues[NavParam.Track.ix][0] = "Unknown";
        displayValues[NavParam.Device.ix][0] = "Unknown";

        // active device that follows UI selection
        cursorTrack = host.createCursorTrack(0, 0);
        cursorDevice = cursorTrack.createCursorDevice();
        remoteControlCursor = cursorDevice.createCursorRemoteControlsPage(NumControls);

        remoteControlCursor.pageNames().addValueObserver((value) -> {
            pageNames = value;
            if (pageIx < 0 || pageNames == null || pageNames.length <= pageIx) {
                return;
            }
            values[NavParam.Page.ix] = (int)(pageIx * 16384.0 / pageNames.length);
            sendParamValue(NavParam.Page.ix, values[NavParam.Page.ix]);
            displayValues[NavParam.Page.ix][0]=pageNames[pageIx];
        });
        remoteControlCursor.selectedPageIndex().addValueObserver((value) -> {
            pageIx = value;
            if (pageIx < 0 || pageNames == null || pageNames.length <= pageIx) {
                return;
            }
            values[NavParam.Page.ix] = (int)(pageIx * 16384.0 / pageNames.length);
            sendParamValue(NavParam.Page.ix, values[NavParam.Page.ix]);
            displayValues[NavParam.Page.ix][0]=pageNames[pageIx];
        });

    }


    @Override
    public void handleValue(int ix, int data1, int data2) {
        NavParam np = NavParam.valueOfIx(ix);
        if (np == null) {
            return;
        }
        switch (np) {
            case Track:
                // TODO: implement
                break;
            case Device:
                // TODO: implement
                break;
            case Page:
                int newPageIx = (int)((values[ix] * pageNames.length) / 16384.0);
                if (newPageIx != pageIx) {
                    pageIx = newPageIx;
                    remoteControlCursor.selectedPageIndex().set(pageIx);
                }
                break;
        }
    }

    public void updateInfoString() {
        if (!active) {
            return;
        }
        sendInfoString("");
    }
}
