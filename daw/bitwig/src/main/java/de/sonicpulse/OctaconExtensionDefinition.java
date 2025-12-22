package de.sonicpulse;

import java.util.UUID;

import com.bitwig.extension.api.PlatformType;
import com.bitwig.extension.controller.AutoDetectionMidiPortNamesList;
import com.bitwig.extension.controller.ControllerExtensionDefinition;
import com.bitwig.extension.controller.api.ControllerHost;

public class OctaconExtensionDefinition extends ControllerExtensionDefinition {
    private static final UUID DRIVER_ID = UUID.fromString("d3e99136-1be1-442d-8c48-9cb422898d93");
    private static final String Model = "Octacon";

    public OctaconExtensionDefinition() {
    }

    @Override
    public String getName() {
        return Model;
    }

    @Override
    public String getAuthor() {
        return "ensonic";
    }

    @Override
    public String getVersion() {
        return "0.1";
    }

    @Override
    public UUID getId() {
        return DRIVER_ID;
    }

    @Override
    public String getHardwareVendor() {
        return "ensonic";
    }

    @Override
    public String getHardwareModel() {
        return Model;
    }

    @Override
    public int getRequiredAPIVersion() {
        return 20;  // keep in sync with pom.xml
    }

    @Override
    public int getNumMidiInPorts() {
        return 1;
    }

    @Override
    public int getNumMidiOutPorts() {
        return 1;
    }

    @Override
    public void listAutoDetectionMidiPortNames(final AutoDetectionMidiPortNamesList list,
            final PlatformType platformType) {
        switch (platformType) {
            case LINUX:
               list.add(new String[] {Model + " MIDI 1"}, new String[] {Model + " MIDI 1"});
               break;
            default:
               list.add(new String[] {Model}, new String[] {Model});
               break;
         }
    }

    @Override
    public String getHelpFilePath() {
        return "https://github.com/ensonic/octacon/blob/main/README.md";
    }

    @Override
    public OctaconExtension createInstance(final ControllerHost host) {
        return new OctaconExtension(this, host);
    }
}
