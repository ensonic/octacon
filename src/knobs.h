// knob handlers code

#ifndef SRC_KNOBS_H
#define SRC_KNOBS_H

#include <EndlessPotentiometer.h>
#include <Mux.h>
#include <mavg.h>
#include <config.h>

using valueCallback_t = void (*)(unsigned ix, int value, int delta);
using buttonCallback_t = void (*)(unsigned ix, int state);

class Knobs {
public:
    Knobs(admux::Mux *vala,admux::Mux *valb, admux::Mux *btn, int8_t *kmap); 

    void begin(void);
    void tick(void);

    void setLimits(int mi, int ma);
    void setValue(int val);
    void setValue(unsigned ix, int val);
    int getValue(unsigned ix);

    void attachValueCallback(valueCallback_t callback);
    void attachButtonCallback(buttonCallback_t callback);

private:
    admux::Mux *vala,*valb, *btn;
    buttonCallback_t bcb;
    valueCallback_t vcb;
    int minV,maxV;
    float scale;
    // ignore changes less than this in the adc range
    int threshold = 14;
    int values[numParams];
    int buttons[numParams];
    // ignore value changes when we turn the know to prevent jumps
    unsigned long lastTs[numParams] = {0,};
    // index mapping
    int8_t *kmap;

    // moving averages for analog reads
    MAvg mavgA[numParams], mavgB[numParams];
    // analog reads to movements
    EndlessPotentiometer pots[numParams];
};

#endif // SRC_KNOBS_H