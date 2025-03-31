// knob handlers code

#include <debug.h>
#include <knobs.h>

#include <math.h>

extern Debug dbg;

static const int ADC_100_PCT = (1<<12);
static const int ADC_50_PCT = (ADC_100_PCT>>1);
static const int ADC_80_PCT = ADC_100_PCT * 0.8;
static const int ADC_55_PCT = ADC_100_PCT * 0.55;
static const int ADC_20_PCT = ADC_100_PCT * 0.2;

Knobs::Knobs(admux::Mux *vala,admux::Mux *valb, admux::Mux *btn) : vala(vala), valb(valb), btn(btn) {}

void Knobs::begin(void) {
    analogReadResolution(12);
    tick();
}

void Knobs::tick(void) {
    for (unsigned i = 0; i < knobCount; i++) {
        // it is enough to drive one multiplexer address, as we use the same 
        // S{0,1,2} pins for all 3 multiplexers
        vala->channel(i);
        delayMicroseconds(1); // let  ADC settle

        auto va = mavgA[i].update(vala->read());
        auto vb = mavgB[i].update(valb->read());

        int delta = handlePot(i, va, vb);
        if (vcb && abs(delta) > 0) {
            int v=values[i];
            if ( v + delta > maxV) {
                dbg.printf("clip upper: %d + %d\n", v, delta);
                delta = maxV - values[i];
            } else if (v + delta < minV) {
                dbg.printf("clip lower: %d + %d\n", v, delta);
                delta = minV - values[i];
            }
            if (abs(delta) > 0) {
                v += delta;
                vcb(i, v, delta);
            }
            values[i] = v;
        }
 
        auto button = btn->read();
        if (bcb && button != buttons[i]) {
            buttons[i] = button;
            bcb(i, button);
        }
    }    
}

void Knobs::setLimits(int mi, int ma) {
    minV = mi;
    maxV = ma;
    scale = (float)ADC_100_PCT/(float)((1+ma)-mi);
}

void Knobs::setValue(int val) {
    for (unsigned i = 0; i < knobCount; i++) {
       values[i] = val;
    }
}

void Knobs::setValue(unsigned ix, int val) {
    values[ix] = val;
}

void Knobs::attachValueCallback(valueCallback_t callback) {
    vcb = callback;
}

void Knobs::attachButtonCallback(buttonCallback_t callback) {
    bcb = callback;
}

// Calculate the position change from previous call.
int Knobs::handlePot(int ix, int va, int vb) {
    // atan2 is meant for cos and sin, but we actually have triangles
    // map them to -1 .. +1 range
    float angle = atan2(
        (float)(vb-ADC_50_PCT)/(float)ADC_50_PCT, 
        (float)(va-ADC_50_PCT)/(float)ADC_50_PCT);
    // map angle from -pi .. pi -> 0 .. ADC_100_PCT (and flip) 
    int value = (int)(float)(ADC_100_PCT * (0.5 - (angle / M_PI)));
    int delta = value - adcValues[ix];
    int adelta = abs(delta);
    if (adelta < 5) { // TODO: add jitter parameter ('5')
        return 0;
    }
    // a/b should be only up to 50% apart
    //delta = (delta < -ADC_50_PCT) ? -ADC_50_PCT : (delta > ADC_50_PCT) ? ADC_50_PCT : delta;
    if (adelta > ADC_55_PCT) {  // TODO: also make this a param?
        return 0;
    }
    //dbg.printf("%4d + %+5d = %4d\n", adcValues[ix], delta, value);
    adcValues[ix] = value;
    return delta / scale;
}