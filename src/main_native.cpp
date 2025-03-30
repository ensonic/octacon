#include "EndlessPotentiometer.h"

const int MAX_ADC_VALUE=(MAX_POT_VALUE-1);
const int MID_ADC_VALUE=(MAX_POT_VALUE/2)-1;
const int MIN_ADC_VALUE=0;
//const int MAX_RES_VALUE=256;
const int MAX_RES_VALUE=4096;
const int MIN_RES_VALUE=0;

void init_pot(EndlessPotentiometer& p) {
    p.threshold = 0;
    p.safetyNet = 100;
    //p.sensitivity = (MAX_POT_VALUE/MAX_RES_VALUE)/2;
    p.sensitivity = 1.0;
    p.minValue = MIN_RES_VALUE;
    p.maxValue = MAX_RES_VALUE-1;
}

void print_csv(void) {
    EndlessPotentiometer p;
    init_pot(p);
    p.dump_csv_hdr();

    int va = MIN_ADC_VALUE, vad =  2;
    int vb = MID_ADC_VALUE, vbd =  2;
    for (int v = 0; v < MAX_POT_VALUE; v++) {
        p.updateValues(va, vb);
        p.dump_csv((v*360.0/(float)MAX_POT_VALUE));

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
    print_csv();
    return 0; 
}
