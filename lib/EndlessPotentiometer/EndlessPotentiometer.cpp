/*
This is from https://github.com/juanlittledevil/EndlessPotentiometer
I rewrote the delta calculations, added the “safety net” and made some
performance improvements. The original doesn't specify a license, I will contact
juanlittledevel and the original author to clear things up.
*/

#include <EndlessPotentiometer.h>
#include <math.h> // for abs()

EndlessPotentiometer::EndlessPotentiometer() {
  valueA = 0;
  valueB = 0;
  valueChanged = 0;
  value = 0;
}

void EndlessPotentiometer::updateValues(int valueA, int valueB) {
  previousValueA = this->valueA;
  previousValueB = this->valueB;

  this->valueA = valueA;
  this->valueB = valueB;

  // decode each wiper value direction.
  if (valueA > (previousValueA + threshold)) {
    dirA = UP;
  } else if (valueA < (previousValueA - threshold)) {
    dirA = DOWN;
  } else {
    dirA = NOT_MOVING;
  }

  if (valueB > (previousValueB + threshold)) {
    dirB = UP;
  } else if (valueB < (previousValueB - threshold)) {
    dirB = DOWN;
  } else {
    dirB = NOT_MOVING;
  }

  // Now evaluate the actual direction of the pot.
  if (dirA == DOWN && dirB == DOWN) {
    if (valueA > valueB) {
      direction = CLOCKWISE;
    } else {
      direction = COUNTER_CLOCKWISE;
    }
  } else if (dirA == UP && dirB == UP) {
    if (valueA < valueB) {
      direction = CLOCKWISE;
    } else {
      direction = COUNTER_CLOCKWISE;
    }
  } else if (dirA == UP && dirB == DOWN) {
    if ((valueA > adcHalfValue) || (valueB > adcHalfValue)) {
      // safety net - TODO: explain
      if (previousValueA < adcHalfValue && (previousValueB < safetyNet || previousValueB > (adcHalfValue - safetyNet))) {
        direction = previousDirection;
      } else {
        direction = CLOCKWISE;
      }
    } else {
      direction = COUNTER_CLOCKWISE;
    }
  } else if (dirA == DOWN && dirB == UP) {
    if ((valueA < adcHalfValue) || (valueB < adcHalfValue)) {
      // safety net - TODO: explain
      if (previousValueA > adcHalfValue && (previousValueB < safetyNet ||  previousValueB > (adcHalfValue - safetyNet))) {
        direction = previousDirection;
      } else {
        direction = CLOCKWISE;
      }
    } else {
      direction = COUNTER_CLOCKWISE;
    }
  } else {
    direction = NOT_MOVING;
  }

  // If we are not not dancing then we are dancing.
  if (direction == NOT_MOVING) {
    isMoving = false;
  } else {
    isMoving = true;
    previousDirection = direction;
  }

  // Record the change.
  // Avoid values around zero and max, as value has a flat region. Instead use
  // the values in between which are more predictable and linear.
  if (isMoving) {
    if ((valueA < adc08Value) && (valueA > adc02Value)) {
      valueChanged = abs(valueA - previousValueA) / sensitivity;
    } else {
      valueChanged = abs(valueB - previousValueB) / sensitivity;
    }
    // Update the value: apply change and restrict to range)
    value = getValue(value);
  } else {
    valueChanged = 0;
  }
}

int EndlessPotentiometer::getValue(int value) {
  if (isMoving) {
    if (direction == CLOCKWISE) {
      value += valueChanged;
    } else if (direction == COUNTER_CLOCKWISE) {
      value -= valueChanged;
    }

    if (value < minValue) {
      value = minValue;
    } else if (value > maxValue) {
      value = maxValue;
    }
  }
  return value;
}

#ifdef DEBUG

#include <stdio.h>

void EndlessPotentiometer::dump(const char *msg) {
  char va=' ', vb=' ';
  if ((valueA < adc08Value) && (valueA > adc02Value)) {
    va='*';
  } else {
    vb='*';
  }
  printf("-- %s --\n", msg);
  printf("moving=%1d, val=%4d[%4d], dir=%+2d[%+2d]\n", isMoving, value, valueChanged, direction, previousDirection);
  printf("A%c: cur=%4d, pre=%4d, dif=%+5d, dir=%+2d\n", va, valueA, previousValueA, (valueA - previousValueA), dirA);
  printf("B%c: cur=%4d, pre=%4d, dif=%+5d, dir=%+2d\n", vb, valueB, previousValueB, (valueB - previousValueB), dirB);
  printf("range=%4d..%4d, sensitivity=%f\n", minValue, maxValue, sensitivity);
}
#endif
