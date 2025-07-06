package de.sonicpulse;

import com.bitwig.extension.controller.api.ControllerHost;

public class Logger {
    private static ControllerHost host = null;

    public static void init(ControllerHost host) {
        Logger.host = host;
    }

    public static void log(String fmt, Object... values) {
        String m = String.format(fmt, values);
        if (host != null) {
            host.println(m);
        }
        java.lang.System.out.println(m);
    }
}
