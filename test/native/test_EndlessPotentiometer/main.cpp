#include <unity.h>
#include <math.h>

#include "EndlessPotentiometer.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

const int MAX_ADC_VALUE=(MAX_POT_VALUE-1);
const int MID_ADC_VALUE=(MAX_POT_VALUE/2)-1;
const int MIN_ADC_VALUE=0;
const int MAX_RES_VALUE=256;
const int MIN_RES_VALUE=0;
const int V0 = MIN_RES_VALUE;
const int V25 = (MAX_RES_VALUE/4)-1;
const int V50 = (MAX_RES_VALUE/2)-1;
const int V75 = (MAX_RES_VALUE/2 + MAX_RES_VALUE/4)-1;
const int V100 = MAX_RES_VALUE-1;
// see comment for sensitivity wrt to the *2
const int step = ((float)MAX_POT_VALUE/(float)MAX_RES_VALUE)*2.0;

void init_pot(EndlessPotentiometer& p) {
    p.sensitivity = ((float)MAX_POT_VALUE/(float)MAX_RES_VALUE)*2.0;
    p.threshold = p.safetyNet = 0; // tests don't jitter
    p.minValue = V0;
    p.maxValue = V100;
    p.value=V0;
    // don't call updateValues() yet, but always call it twice in the tests
}

void test_0_degrees() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V0;
    p.updateValues(MIN_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MIN_ADC_VALUE, MID_ADC_VALUE);
    TEST_ASSERT_EQUAL_INT(V0, p.value);
    TEST_ASSERT_EQUAL_INT(0, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::NOT_MOVING, p.direction);
    TEST_ASSERT_FALSE(p.isMoving);
}

void test_0_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V0;
    p.updateValues(MIN_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MIN_ADC_VALUE-step, MID_ADC_VALUE-step);
    TEST_ASSERT_EQUAL_INT(V0, p.value); // no wrap around!
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::COUNTER_CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_0_degrees_cw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V0;
    p.updateValues(MIN_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MIN_ADC_VALUE+step, MID_ADC_VALUE+step);
    TEST_ASSERT_EQUAL_INT(V0+1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_90_degrees() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V25;
    p.updateValues(MID_ADC_VALUE, MAX_ADC_VALUE);
    p.updateValues(MID_ADC_VALUE, MAX_ADC_VALUE);
    TEST_ASSERT_EQUAL_INT(V25, p.value);
    TEST_ASSERT_EQUAL_INT(0, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::NOT_MOVING, p.direction);
    TEST_ASSERT_FALSE(p.isMoving);
}

void test_90_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V25;
    p.updateValues(MID_ADC_VALUE, MAX_ADC_VALUE);
    p.updateValues(MID_ADC_VALUE-step, MAX_ADC_VALUE-step);
    TEST_ASSERT_EQUAL_INT(V25-1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::COUNTER_CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_90_degrees_cw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V25;
    p.updateValues(MID_ADC_VALUE, MAX_ADC_VALUE);
    p.updateValues(MID_ADC_VALUE+step, MAX_ADC_VALUE-step);
    TEST_ASSERT_EQUAL_INT(V25+1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_180_degrees() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V50;
    p.updateValues(MAX_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MAX_ADC_VALUE, MID_ADC_VALUE);
    TEST_ASSERT_EQUAL_INT(V50, p.value);
    TEST_ASSERT_EQUAL_INT(0, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::NOT_MOVING, p.direction);
    TEST_ASSERT_FALSE(p.isMoving);
}

void test_180_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V50;
    p.updateValues(MAX_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MAX_ADC_VALUE-step, MID_ADC_VALUE+step);
    TEST_ASSERT_EQUAL_INT(V50-1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::COUNTER_CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_180_degrees_cw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V50;
    p.updateValues(MAX_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MAX_ADC_VALUE-step, MID_ADC_VALUE-step);
    TEST_ASSERT_EQUAL_INT(V50+1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_270_degrees() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V75;
    p.updateValues(MID_ADC_VALUE, MIN_ADC_VALUE);
    p.updateValues(MID_ADC_VALUE, MIN_ADC_VALUE);
    TEST_ASSERT_EQUAL_INT(V75, p.value);
    TEST_ASSERT_EQUAL_INT(0, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::NOT_MOVING, p.direction);
    TEST_ASSERT_FALSE(p.isMoving);
}

void test_270_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V75;
    p.updateValues(MID_ADC_VALUE, MIN_ADC_VALUE);
    p.updateValues(MID_ADC_VALUE-step, MIN_ADC_VALUE+step);
    TEST_ASSERT_EQUAL_INT(V75-1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::COUNTER_CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_270_degrees_cw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V75;
    p.updateValues(MID_ADC_VALUE, MIN_ADC_VALUE);
    p.updateValues(MID_ADC_VALUE+step, MIN_ADC_VALUE+step);
    TEST_ASSERT_EQUAL_INT(V75+1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_360_degrees() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V100;
    p.updateValues(MIN_ADC_VALUE, MID_ADC_VALUE);
    TEST_ASSERT_EQUAL_INT(V100, p.value);
    TEST_ASSERT_EQUAL_INT(0, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::NOT_MOVING, p.direction);
    TEST_ASSERT_FALSE(p.isMoving);
}

void test_360_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V100;
    p.updateValues(MIN_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MIN_ADC_VALUE+step, MID_ADC_VALUE-step);
    TEST_ASSERT_EQUAL_INT(V100-1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::COUNTER_CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_360_degrees_cw() {
    EndlessPotentiometer p;
    init_pot(p);

    p.value=V100;
    p.updateValues(MIN_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MIN_ADC_VALUE+step, MID_ADC_VALUE+step);
    TEST_ASSERT_EQUAL_INT(V100, p.value); // no wrap around!
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    // test all combinations in 60° steps
    RUN_TEST(test_0_degrees);
    RUN_TEST(test_0_degrees_ccw);
    RUN_TEST(test_0_degrees_cw);
    RUN_TEST(test_90_degrees);
    RUN_TEST(test_90_degrees_ccw);
    RUN_TEST(test_90_degrees_cw);
    RUN_TEST(test_180_degrees);
    RUN_TEST(test_180_degrees_ccw);
    RUN_TEST(test_180_degrees_cw);
    RUN_TEST(test_270_degrees);
    RUN_TEST(test_270_degrees_ccw);
    RUN_TEST(test_270_degrees_cw);
    RUN_TEST(test_360_degrees);
    RUN_TEST(test_360_degrees_ccw);
    RUN_TEST(test_360_degrees_cw);
    UNITY_END();
}