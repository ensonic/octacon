// knob handlers code

#include <knobs.h>

Knobs::Knobs(admux::Mux *vala,admux::Mux *valb, admux::Mux *btn) : vala(vala), valb(valb), btn(btn) {}

void Knobs::begin(void) {
    analogReadResolution(12);
}

void Knobs::tick(void) {
    for (unsigned i = 0; i < knobCount; i++) {
        // it is enough to drive one multiplexer address, as we use the same 
        // S{0,1,2} pins for all 3 multiplexers
        vala->channel(i);
        delayMicroseconds(1); // let  ADC settle

        auto va = mavgA[i].update(vala->read());
        auto vb = mavgB[i].update(valb->read());
        auto p = pots[i];
        p.updateValues(va, vb);
        if (vcb && p.isMoving) {
            switch (p.direction) {
                case p.CLOCKWISE:
                    vcb(i, p.value, p.valueChanged);
                    break;
                case p.COUNTER_CLOCKWISE:
                    vcb(i, p.value, -p.valueChanged);
                    break;
            }
        }

        auto button = btn->read();
        if (bcb && button != buttons[i]) {
            buttons[i] = button;
            bcb(i, button);
        }
    }    
}

void Knobs::setLimits(int mi, int ma) {
    for (unsigned i = 0; i < knobCount; i++) {
        pots[i].minValue = mi;
        pots[i].maxValue = ma;
    }
}

void Knobs::setValue(int val) {
    for (unsigned i = 0; i < knobCount; i++) {
        pots[i].value = val;
    }
}

void Knobs::setValue(unsigned ix, int val) {
    pots[ix].value = val;
}

void Knobs::attachValueCallback(valueCallback_t callback) {
    vcb = callback;
}

void Knobs::attachButtonCallback(buttonCallback_t callback) {
    bcb = callback;
}
