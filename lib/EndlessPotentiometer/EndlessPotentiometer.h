/*
This is from https://github.com/juanlittledevil/EndlessPotentiometer
I rewrote the delta calculations, added the “safety net” and made some
performance improvements. The original doesn't specify a license, I will contact
juanlittledevel and the original author to clear things up.
*/
// EndlessPotentiometer.h

#ifndef EndlessPotentiometer_h
#define EndlessPotentiometer_h

// You might need a analogReadResolution(12); to activate this
const int MAX_POT_VALUE=4096; // 12 bit

class EndlessPotentiometer {
public:
  enum Direction { NOT_MOVING, CLOCKWISE, COUNTER_CLOCKWISE };

  bool isMoving = false;
  int direction = NOT_MOVING; // Which direction the knob is being turned.
  int previousDirection = NOT_MOVING;
  // Values are in the min-max value range (see sensitivity)
  int value = 0;        // Current knob value.
  int valueChanged = 0; // Amount of change between last collection and
                        // the current one

  // Threshold is the amount of change that must happen in order to be
  // considered as a change in value. Ideally this should be set low but just
  // large enough to catch jitter from the pot. 3 was a good value in my tests.
  // This is specifies in the source range (raw adc value).
  int threshold = 3;
  // Don't change direction if we just crossed the zero point on the pot
  int safetyNet = 400;

  // Sensitivity refers to how quickly values change when the knob is turned.
  // Higher values will make it less sensitivity thus, needing to turn the knob
  // more times until to reach the max or min values. If you are working with
  // 127 values, a larger number may be desired. However, if you are working
  // with larger values you will want to set this lower.
  // Use 2.0*((float)MAX_POT_VALUE/(float)maxValue) as one knob rotation (360°) will go from 
  // 0 to MAX_POT_VALUE and back
  float sensitivity = 8;

  // Sets the max value range. That the internal max value can be set to. The
  // default is 127, but this can be scaled ot anything you want.
  int maxValue = 127;

  // Sets the min value for the range. can be anything as long as it is less
  // than maxValue
  int minValue = 0;

  EndlessPotentiometer();

  // You MUST include this in your loop() function, and it must be called before
  // accessing any other variable of method from this library in your sketch.
  void updateValues(int valueA, int valueB);

  // Use your own value instead of the internal one.
  int getValue(int value);

#ifdef DEBUG
  // Dump internal state to stdout
  void dump_pretty(const char *msg);
  void dump_csv_hdr(void);
  void dump_csv(float ang);
#endif

private:
  enum IndividualDirection { UP = 1, DOWN = -1 };

  int valueA;
  int valueB;
  int previousValueA = 0;
  int previousValueB = 0;
  int dirA = NOT_MOVING;
  int dirB = NOT_MOVING;
  const int adcMaxValue =
      MAX_POT_VALUE; // the max value from analogRead for each pin.
  const int adcHalfValue = adcMaxValue / 2;
  const int adc08Value = adcMaxValue * 0.8;
  const int adc02Value = adcMaxValue * 0.2;
};

#endif
