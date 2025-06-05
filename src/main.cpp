// octacon midi controller

#include <config.h>

// hardware variant to build
#if HW_VER == 1
#include <hw.v1.h>
#elif HW_VER == 2
#include <hw.v2.h>
#endif


static boolean active = false;

// callbacks

static void valueCB(unsigned int ix,int value,int delta) {
    if (!active) return;
    dbg.printf("Knob[%u]: Value = %d | Delta = %d\n", ix, value, delta);
    ui.setValue(ix, value);
    mio.sendValueCC(ix, value);
}

static void buttonCB(unsigned int ix, int state) {
    if (!active) return;
    dbg.printf("Button:[%u]; State= %d\n", ix, state);
    mio.sendButtonCC(ix, state*64);
}

void setup() {
    unsigned long ts0 = millis();
    leds.begin(); // call early to reset to black

    dbg.init();
    dbg.println("Setup start");

    pinMode(LED_BUILTIN, OUTPUT);

    ui.init();

    knobs.setLimits(0,VAL_MAX);
    knobs.setValue(VAL_MAX/2);
    knobs.attachButtonCallback(buttonCB);
    knobs.attachValueCallback(valueCB);
    knobs.begin();

    mio.init();

    ui.begin();

    capsense.begin();

    leds.SetColors(0.05);
    dbg.printf("Setup done: %u ms\n", millis()-ts0);
}

// run midi at full speed
// don't poll the knobs too quickly though
void loop() {
    unsigned long m=millis();
    static unsigned long tk = 0;
    if (m - tk > 20) {
        knobs.tick();
        tk = m;
    }

    mio.tick();

    static unsigned long tb = 0;
    static bool blink = false;
    if (m - tb > 500)  {
        digitalWrite(LED_BUILTIN, blink);
        blink = !blink;
        tb = m;
        /* DEBUG
        leds.SetColors((blink ? 0.2 : 0.3));
        */
    }

    static unsigned long tt = 0;
    static unsigned long touch_wait = 0;
    static boolean touched = false;
    if (m - tt > touch_wait)  {
        touch_wait = 0;
        // csv is the discharge time, if long, the sensor has been touched
        unsigned long csv = capsense.touch();
        //dbg.printf("CapSensor: %lf µs\n", csv);
        if (!touched && csv > 1000) {
            touched = true;
            touch_wait = 250;
            dbg.printf("CapSensor: touch    : %lu µs\n", csv);
            active = !active;
            leds.SetColors(active ? 0.3 : 0.05);
        } else if (touched && csv < 250) {
            touched = false;
            touch_wait = 250;
            dbg.printf("CapSensor: released : %lu µs\n", csv);
        }
        tt = m;
    }
}
