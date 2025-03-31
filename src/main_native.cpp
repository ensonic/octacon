/*
pio run -e desktop
./.pio/build/desktop/program x >~/Temp/endlesspots.csv
*/

#include "EndlessPotentiometer.h"

/*
#include <stdio.h>
#include <math.h>
*/

const int MAX_ADC_VALUE=(MAX_POT_VALUE-1);
const int MID_ADC_VALUE=(MAX_POT_VALUE/2)-1;
const int MIN_ADC_VALUE=0;
//const int MAX_RES_VALUE=256;
const int MAX_RES_VALUE=4096;
const int MIN_RES_VALUE=0;

void init_pot(EndlessPotentiometer& p) {
    p.threshold = 0;
    p.safetyNet = 0;
    p.sensitivity = ((float)MAX_POT_VALUE/(float)MAX_RES_VALUE)*2.0;
    p.minValue = MIN_RES_VALUE;
    p.maxValue = MAX_RES_VALUE-1;
}

void print_csv(void) {
    int ss=16; // step size
    EndlessPotentiometer p;
    init_pot(p);
    p.dump_csv_hdr();

    int va = MIN_ADC_VALUE, vb = MID_ADC_VALUE;
    int vad = 2*ss, vbd = 2*ss;
    p.updateValues(va, vb);
    // turn cw
    for (int v = 0; v < MAX_POT_VALUE; v+=ss) {
        p.updateValues(va, vb);
        float in_angle = v*360.0/(float)MAX_POT_VALUE;

        /* thats basically what we emulate
        float out_angle=atan2(
            (float)(vb-MID_ADC_VALUE)/(float)MID_ADC_VALUE, 
            (float)(va-MID_ADC_VALUE)/(float)MID_ADC_VALUE);
        printf("%lf,%lf\n", in_angle, out_angle);
        */

        p.dump_csv(in_angle);

        va += vad;
        if (va >= MAX_ADC_VALUE || va <= MIN_ADC_VALUE) {
            vad = -vad;
        }
        vb += vbd;
        if (vb >= MAX_ADC_VALUE || vb <= MIN_ADC_VALUE) {
            vbd = -vbd;
        }
    }
}
int main(int argc, char **argv) {
    if (argc > 1) {
        print_csv();
    }
    return 0; 
}
