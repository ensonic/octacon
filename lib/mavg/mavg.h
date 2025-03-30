// simple moving average

#ifndef SRC_MAVG_H
#define SRC_MAVG_H

class MAvg {
public:
    MAvg();

    // push in newValue, returns new average
    int update(int newValue);

private:
    const static unsigned bits = 2;
    const static unsigned size = 1 << bits;
    const static unsigned ixMask = size - 1;
    int hist[size] = {0,};
    int accu = 0;
    int ix = 0;
};

#endif // SRC_MAVG_H