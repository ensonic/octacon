/*
pio run -e desktop
./.pio/build/desktop/program x >~/Temp/endlesspots.tsv
*/

#include "EndlessPotentiometer.h"
#include <stdio.h>

void print_csv(void) {
    float ss=1.0/360.0; // step size
    EndlessPotentiometer p;

    printf("angle\tva\tvb\tv\n");

    float va = -1.0, vb = 0.0;
    float vad = 2*ss, vbd = 2*ss;
    float v;
    p.update(va, vb);
    // turn cw
    for (float i = 0; i < 1.0; i+=ss) {
        v = p.update(va, vb);
        float in_angle = i*360.0;

        /* thats basically what we emulate
        float out_angle=atan2(
            (float)(vb-MID_ADC_VALUE)/(float)MID_ADC_VALUE, 
            (float)(va-MID_ADC_VALUE)/(float)MID_ADC_VALUE);
        printf("%lf,%lf\n", in_angle, out_angle);
        */

        printf("%4.1f\t%6.3f\t%6.3f\t%6.3f\n", in_angle, va, vb, v);

        va += vad;
        if (va >= 1.0 || va <= -1.0) {
            vad = -vad;
        }
        vb += vbd;
        if (vb >= 1.0 || vb <= -1.0) {
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
