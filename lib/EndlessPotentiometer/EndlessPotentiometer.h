// logic for handling dual wiper pots

#ifndef EndlessPotentiometer_h
#define EndlessPotentiometer_h

class EndlessPotentiometer {
public:
    EndlessPotentiometer();

    float update(float va, float vb);

    // ignore changes less than this in the 0.0 ... 1.0 range
    float threshold = 0;
private:
    float last = 0.0;
};

#endif // EndlessPotentiometer_h
