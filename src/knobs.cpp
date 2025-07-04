// knob handlers code

#include <debug.h>
#include <knobs.h>

#include <math.h>

extern Debug dbg;

// use analogReadResolution(12); to activate this resolution, default is 10
static const int ADC_100_PCT = (1<<12);
static const int ADC_50_PCT = (ADC_100_PCT>>1);

/* which pot to debug (press button to switch)
static int dbgix=0;
static int vami=ADC_100_PCT,vama=0;
static int vbmi=ADC_100_PCT,vbma=0;
*/

Knobs::Knobs(admux::Mux *vala,admux::Mux *valb, admux::Mux *btn, int8_t *kmap) : vala(vala), valb(valb), btn(btn) , kmap(kmap) {
    float threshold_pct = (float)threshold / (float)ADC_100_PCT;
    for (unsigned i = 0; i < numParams; i++) {
        pots[i].threshold = threshold_pct;
    }
}

void Knobs::begin(void) {
    analogReadResolution(12);
    tick();
}

void Knobs::tick(void) {
    for (unsigned i = 0; i < numParams; i++) {
        // it is enough to drive one multiplexer address, as we use the same 
        // S{0,1,2} pins for all 3 multiplexers
        /*
        */
        vala->channel(kmap[i]);
        delayMicroseconds(1); // let  ADC settle

        auto va = mavgA[i].update(vala->read());
        auto vb = mavgB[i].update(valb->read());
       
        // see plot.py
        /*if (i==0) {
            dbg.printf("plt:%d,%d\n", va, vb);
        }*/
        /* DEBUG
        if (i == dbgix) {
            if (va > vama) vama=va;
            else if (va < vami) vami=va;
            if (vb > vbma) vbma=vb;
            else if (vb < vbmi) vbmi=vb;
        } */

        float vaf = (float)(va-ADC_50_PCT)/(float)ADC_50_PCT;
        float vbf = (float)(vb-ADC_50_PCT)/(float)ADC_50_PCT;
        int delta = scale * pots[i].update(vaf, vbf);
        if (vcb && abs(delta) > 0) {
            lastTs[i] = millis();
            int v=values[i];
            if ( v + delta > maxV) {
                //dbg.printf("clip upper: %d + %d\n", v, delta);
                delta = maxV - values[i];
            } else if (v + delta < minV) {
                //dbg.printf("clip lower: %d + %d\n", v, delta);
                delta = minV - values[i];
            }
            if (abs(delta) > 0) {
                v += delta;
                vcb(i, v, delta);
                /* DEBUG
                if (i == dbgix) {
                    dbg.printf("%1d: v=%5d, d=%+5d, va=%5d..%5d..%5d, vb=%5d..%5d..%5d\n",
                        i, v, delta, vami,va, vama, vbmi, vb, vbma);
                } */
            }
            values[i] = v;
        }
 
        auto button = btn->read();
        if (bcb && button != buttons[i]) {
            buttons[i] = button;
            bcb(i, button);
            /* DEBUG
            if (button == 0) {
                vami=vbmi=ADC_100_PCT;
                vama=vbma=0;
                dbgix = i;
            } */
        }
    }    
}

void Knobs::setLimits(int mi, int ma) {
    minV = mi;
    maxV = ma;
    scale = (float)((1+ma)-mi);
}

void Knobs::setValue(int val) {
    for (unsigned i = 0; i < numParams; i++) {
       values[i] = val;
    }
}

void Knobs::setValue(unsigned ix, int val) {
    if ((millis() - lastTs[ix]) < 100) return;

    values[ix] = val;
}

int Knobs::getValue(unsigned ix) {
    return values[ix];
}

void Knobs::attachValueCallback(valueCallback_t callback) {
    vcb = callback;
}

void Knobs::attachButtonCallback(buttonCallback_t callback) {
    bcb = callback;
}
