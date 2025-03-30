// simple moving average


#include <mavg.h>

MAvg::MAvg() {}

int MAvg::update(int newValue) {
    int oldValue = hist[ix];
    accu = (accu - oldValue) + newValue;
    hist[ix] = newValue;
    ix = (ix + 1) & ixMask;
    return accu >> bits;
}