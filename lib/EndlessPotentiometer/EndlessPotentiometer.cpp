// logic for handling dual wiper pots

#include <EndlessPotentiometer.h>
#include <math.h> // for fabs, atan2

static const float PI2 = M_PI + M_PI;

EndlessPotentiometer::EndlessPotentiometer() {}

float EndlessPotentiometer::update(float va, float vb) {
    // atan2 is meant for cos and sin, but we actually have triangles
    // map them to -1 .. +1 range
    // the function takes (y,x) but it doesn't mater as we care about the delta
    float angle = atan2f(vb, va);
    // map angle from -pi .. pi -> 0.0 .. 1.0 (and flip) 
    float value = 0.5 - (angle / PI2);
    float delta = value - last;
    float adelta = fabs(delta);
    if (adelta < threshold) { // handle jitter
        return 0.0;
    }
    if (adelta > 0.55) { // handle 360° - 0° transition.
        delta = 1.0 - adelta;
    }
    last = value; 
    return delta;
}
