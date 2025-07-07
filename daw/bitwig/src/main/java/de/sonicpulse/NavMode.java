package de.sonicpulse;

import com.bitwig.extension.controller.api.ControllerHost;
import com.bitwig.extension.controller.api.CursorDevice;
import com.bitwig.extension.controller.api.CursorRemoteControlsPage;
import com.bitwig.extension.controller.api.CursorTrack;
import com.bitwig.extension.controller.api.DeviceBank;
import com.bitwig.extension.controller.api.TrackBank;

public class NavMode extends Mode {
    private CursorTrack cursorTrack;
    private CursorDevice cursorDevice;
    private CursorRemoteControlsPage remoteControlCursor;
    private TrackBank trackBank;
    private DeviceBank deviceBank;
    private String[] pageNames = null;
    private int pageIx = -1, trackIx = -1, deviceIx = -1;
    private int trackSize = 0, deviceSize = 0;

    private enum NavParam {
        Track(0, "Track"),
        Device(4, "Device"),
        /* Nested Chain: cursorDevice.hasSlots() cursorDevice.slotNames() */
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
        for (NavParam np : NavParam.values()) {
            names[np.ix] = np.label;
        }

        // active device that follows UI selection
        cursorTrack = host.createCursorTrack(0, 0);
        cursorDevice = cursorTrack.createCursorDevice();
        remoteControlCursor = cursorDevice.createCursorRemoteControlsPage(NumControls);

        // Track navigation
        trackBank = host.createTrackBank(1,1,0, true);
        trackBank.followCursorTrack(cursorTrack);
        cursorTrack.name().addValueObserver((value) -> {
            displayValues[NavParam.Track.ix][0]=value;
        });
        cursorTrack.position().addValueObserver((value) -> {
            trackIx = value;
            Logger.log("Track: %d/%d", trackIx, trackSize);
            if (trackIx < 0 || trackSize <= trackIx) {
                return;
            }
            // this counts different that the flattened trackBank
            trackBank.scrollIntoView(trackIx+1);
            values[NavParam.Track.ix] = (int)(trackIx * 16384.0 / trackSize);
            sendParamValue(NavParam.Track.ix, values[NavParam.Track.ix]);
        });
        trackBank.itemCount().addValueObserver((value) -> {
            trackSize = value;
            Logger.log("Track: %d/%d", trackIx, trackSize);
            if (trackIx < 0 || trackSize <= trackIx) {
                return;
            }
            values[NavParam.Track.ix] = (int)(trackIx * 16384.0 / trackSize);
            sendParamValue(NavParam.Track.ix, values[NavParam.Track.ix]);
        });

        // Device navigation
        deviceBank = cursorDevice.deviceChain().createDeviceBank(1);
        // API does not exist
        // deviceBank.followCursorDevice(cursorDevice);
        cursorDevice.name().addValueObserver((value)->{
            displayValues[NavParam.Device.ix][0]=value;
        });
        cursorDevice.position().addValueObserver((value) -> {
            deviceIx = value;
            Logger.log("Device: %d/%d", deviceIx, deviceSize);
            if (deviceIx < 0 || deviceSize <= pageIx) {
                return;
            }
            deviceBank.scrollIntoView(deviceIx+1);
            values[NavParam.Device.ix] = (int)(deviceIx * 16384.0 / deviceSize);
            sendParamValue(NavParam.Device.ix, values[NavParam.Device.ix]);
        });
        deviceBank.itemCount().addValueObserver((value) -> {
            deviceSize = value;
            Logger.log("Device: %d/%d", deviceIx, deviceSize);
            if (deviceIx < 0 || deviceSize <= pageIx) {
                return;
            }
            values[NavParam.Device.ix] = (int)(deviceIx * 16384.0 / deviceSize);
            sendParamValue(NavParam.Device.ix, values[NavParam.Device.ix]);
        });

        // Page navigation
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
        int newIx;
        switch (np) {
            case Track:
                newIx = (int)((values[ix] * trackSize) / 16384.0);
                if (newIx != trackIx) {
                    trackIx = newIx;
                    trackBank.scrollIntoView(trackIx+1);
                }
                break;
            case Device:
                newIx = (int)((values[ix] * deviceSize) / 16384.0);
                if (newIx != deviceIx) {
                    Logger.log("change device from %d -> %d", deviceIx, newIx);
                    int delta = newIx - deviceIx;
                    deviceIx = newIx;
                    deviceBank.scrollIntoView(deviceIx+1);
                    // Error: This bank is not following any cursor
                    // deviceBank.cursorIndex().set(0);
                    deviceBank.scrollBy(delta);
                }
                break;
            case Page:
                newIx = (int)((values[ix] * pageNames.length) / 16384.0);
                if (newIx != pageIx) {
                    pageIx = newIx;
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
