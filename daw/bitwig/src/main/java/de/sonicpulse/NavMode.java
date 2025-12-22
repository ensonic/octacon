package de.sonicpulse;

import com.bitwig.extension.controller.api.ControllerHost;
import com.bitwig.extension.controller.api.CursorDevice;
import com.bitwig.extension.controller.api.CursorRemoteControlsPage;
import com.bitwig.extension.controller.api.CursorTrack;
import com.bitwig.extension.controller.api.DeviceBank;
import com.bitwig.extension.controller.api.LastClickedParameter;
import com.bitwig.extension.controller.api.Parameter;
import com.bitwig.extension.controller.api.TrackBank;

// Use the knobs to select a different page, track, device
// Note: only page nave work satisfactory
public class NavMode extends Mode {
    private CursorTrack cursorTrack;
    private CursorDevice cursorDevice;
    private CursorRemoteControlsPage remoteControlCursor;
    private TrackBank trackBank;
    private DeviceBank deviceBank;
    private String[] pageNames = null;
    private int pageIx = -1, trackIx = -1, deviceIx = -1;
    private int trackSize = 0, deviceSize = 0;
    private Parameter lastParam;

    private static final int maxTracksPerBank = 100;
    private static final int maxDevicesPerBank = 100;

    private enum NavParam {
        Track(0, "Track"),
        LastPar(3,""),
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

        // last clicked parameter (actually "last hovered")
        final LastClickedParameter lcp = host.createLastClickedParameter("OctaconLastClickedParam", "Octacon Last Clicked");
        lastParam =  lcp.parameter();
        lastParam.value().addValueObserver(16384, (value) -> {
            sendParamValue(NavParam.LastPar.ix, value);
            values[NavParam.LastPar.ix] = value;
        });
        lastParam.name().addValueObserver((value) -> {
            sendParamName(NavParam.LastPar.ix, value);
            names[NavParam.LastPar.ix]=value;
        });
        lastParam.displayedValue().addValueObserver((value) -> {
                displayValues[NavParam.LastPar.ix][0] = value;
            });
        lastParam.discreteValueCount().addValueObserver((value) -> {
            ticks[NavParam.LastPar.ix] = Math.max(0, value);
            sendParamTicks(NavParam.LastPar.ix, ticks[NavParam.LastPar.ix]);
        });

        // active device that follows UI selection
        cursorTrack = host.createCursorTrack(0, 0);
        cursorDevice = cursorTrack.createCursorDevice();
        remoteControlCursor = cursorDevice.createCursorRemoteControlsPage(NumControls);

        // Track navigation
        trackBank = host.createTrackBank(maxTracksPerBank,1,0, true);
        // this causes erratic jumping when navigating.
        //trackBank.followCursorTrack(cursorTrack);
        cursorTrack.name().addValueObserver((value) -> {
            displayValues[NavParam.Track.ix][0]=value;
        });
        trackBank.itemCount().addValueObserver((value) -> {
            trackSize = value;
            Logger.log("Track(Ct): %d/%d", trackIx, trackSize);
            if (trackSize < 0) {
                ticks[NavParam.Track.ix] = 0;
                sendParamTicks(NavParam.Track.ix, ticks[NavParam.Track.ix]);
                return;
            }
            ticks[NavParam.Track.ix] = trackSize;
            sendParamTicks(NavParam.Track.ix, ticks[NavParam.Track.ix]);
            if (trackIx < 0 || trackSize <= trackIx) {
                return;
            }
            values[NavParam.Track.ix] = (int)(trackIx * 16384.0 / trackSize);
            sendParamValue(NavParam.Track.ix, values[NavParam.Track.ix]);
        });
        trackBank.cursorIndex().addValueObserver((value) -> {
            trackIx = value;
            Logger.log("Track(Ix): %d/%d", trackIx, trackSize);
            if (trackSize < 0 || trackIx < 0 || trackSize <= trackIx) {
                values[NavParam.Track.ix] = 0;
                sendParamValue(NavParam.Track.ix, values[NavParam.Track.ix]);
                return;
            }
            values[NavParam.Track.ix] = (int)(trackIx * 16384.0 / trackSize);
            sendParamValue(NavParam.Track.ix, values[NavParam.Track.ix]);
        });

        // Device navigation
        deviceBank = cursorDevice.deviceChain().createDeviceBank(maxDevicesPerBank);
        // API does not exist
        // deviceBank.followCursorDevice(cursorDevice);
        cursorDevice.name().addValueObserver((value)->{
            displayValues[NavParam.Device.ix][0]=value;
        });
        deviceBank.itemCount().addValueObserver((value) -> {
            deviceSize = value;
            Logger.log("Device(Ct): %d/%d", deviceIx, deviceSize);
            if (deviceSize < 0) {
                ticks[NavParam.Device.ix] = 0;
                sendParamTicks(NavParam.Device.ix, ticks[NavParam.Device.ix]);
                return;
            }
            ticks[NavParam.Device.ix] = deviceSize;
            sendParamTicks(NavParam.Device.ix, ticks[NavParam.Device.ix]);
            if (deviceIx < 0 || deviceSize <= deviceIx) {
                return;
            }
            values[NavParam.Device.ix] = (int)(deviceIx * 16384.0 / deviceSize);
            sendParamValue(NavParam.Device.ix, values[NavParam.Device.ix]);
        });
        cursorDevice.position().addValueObserver((value) -> {
            deviceIx = value;
            Logger.log("Device(Ix): %d/%d", deviceIx, deviceSize);
            if (deviceSize < 0 || deviceIx < 0 || deviceSize <= deviceIx) {
                values[NavParam.Device.ix] = 0;
                sendParamValue(NavParam.Device.ix, values[NavParam.Device.ix]);
                return;
            }
            values[NavParam.Device.ix] = (int)(deviceIx * 16384.0 / deviceSize);
            sendParamValue(NavParam.Device.ix, values[NavParam.Device.ix]);
        });

        // Page navigation
        remoteControlCursor.pageNames().addValueObserver((value) -> {
            pageNames = value;
            if (pageIx < 0 || pageNames == null || pageNames.length <= pageIx) {
                ticks[NavParam.Page.ix] = 0;
                sendParamTicks(NavParam.Page.ix, ticks[NavParam.Page.ix]);
                return;
            }
            values[NavParam.Page.ix] = (int)(pageIx * 16384.0 / pageNames.length);
            sendParamValue(NavParam.Page.ix, values[NavParam.Page.ix]);
            displayValues[NavParam.Page.ix][0]=pageNames[pageIx];
            ticks[NavParam.Page.ix] = pageNames.length;
            sendParamTicks(NavParam.Page.ix, ticks[NavParam.Page.ix]);
        });
        remoteControlCursor.selectedPageIndex().addValueObserver((value) -> {
            pageIx = value;
            if (pageIx < 0 || pageNames == null || pageNames.length <= pageIx) {
                values[NavParam.Page.ix] = 0;
                sendParamValue(NavParam.Page.ix, values[NavParam.Page.ix]);
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
                    Logger.log("change track from %d -> %d", trackIx, newIx);
                    int delta = newIx - trackIx;
                    trackIx = newIx;
                    trackBank.scrollIntoView(trackIx);
                    if (delta > 0) {
                        cursorTrack.selectNext();
                    } else {
                        cursorTrack.selectPrevious();
                    }
                }
                break;
            case LastPar:
                lastParam.set(values[ix] / 16384.0);
                break;
            case Device:
                newIx = (int)((values[ix] * deviceSize) / 16384.0);
                if (newIx != deviceIx) {
                    Logger.log("change device from %d -> %d", deviceIx, newIx);
                    deviceIx = newIx;
                    deviceBank.scrollIntoView(deviceIx);
                    cursorDevice.selectDevice(deviceBank.getDevice(deviceIx));
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
