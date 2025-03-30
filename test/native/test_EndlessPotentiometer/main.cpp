#include <unity.h>
#include "EndlessPotentiometer.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

const int MAX_ADC_VALUE=MAX_POT_VALUE;
const int MID_ADC_VALUE=MAX_ADC_VALUE/2;
const int MIN_ADC_VALUE=0;
const int MAX_RES_VALUE=256;
const int MIN_RES_VALUE=0;
const int V0 = MIN_RES_VALUE;
const int V25 = MAX_RES_VALUE/4;
const int V50 = MAX_RES_VALUE/2;
const int V75 = MAX_RES_VALUE/2 + MAX_RES_VALUE/4;
const int V100 = MAX_RES_VALUE;
// TODO: why do we need to multiply by 2 ??
const int step = (MAX_ADC_VALUE/MAX_RES_VALUE)*2;

void init_pot(EndlessPotentiometer& p) {
    p.sensitivity = step;
    p.minValue = MIN_RES_VALUE;
    p.maxValue = MAX_RES_VALUE;
}

void test_0_degrees() {
    EndlessPotentiometer p;
    init_pot(p);

    p.updateValues(MIN_ADC_VALUE, MID_ADC_VALUE);
    TEST_ASSERT_EQUAL_INT(V0, p.value);
    TEST_ASSERT_EQUAL_INT(0, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::NOT_MOVING, p.direction);
    TEST_ASSERT_FALSE(p.isMoving);
}

void test_0_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

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

    p.updateValues(MIN_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MIN_ADC_VALUE+step, MID_ADC_VALUE+step);
    TEST_ASSERT_EQUAL_INT(V0+1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_90_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

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

    p.updateValues(MID_ADC_VALUE, MAX_ADC_VALUE);
    p.updateValues(MID_ADC_VALUE+step, MAX_ADC_VALUE-step);
    TEST_ASSERT_EQUAL_INT(V25+1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_180_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

    // test fails if we don't move towards the init in smaller steps
    p.updateValues(MID_ADC_VALUE, MAX_ADC_VALUE);
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

    // test fails if we don't move towards the init in smaller steps
    p.updateValues(MID_ADC_VALUE, MAX_ADC_VALUE);
    p.updateValues(MAX_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MAX_ADC_VALUE-step, MID_ADC_VALUE-step);
    TEST_ASSERT_EQUAL_INT(V50+1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_270_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

    // test fails if we don't move towards the init in smaller steps
    //p.updateValues(MID_ADC_VALUE, MAX_ADC_VALUE);
    p.updateValues(MAX_ADC_VALUE, MID_ADC_VALUE);
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

    // test fails if we don't move towards the init in smaller steps
    //p.updateValues(MID_ADC_VALUE, MAX_ADC_VALUE);
    p.updateValues(MAX_ADC_VALUE, MID_ADC_VALUE);
    p.updateValues(MID_ADC_VALUE, MIN_ADC_VALUE);
    p.updateValues(MID_ADC_VALUE+step, MIN_ADC_VALUE+step);
    TEST_ASSERT_EQUAL_INT(V75+1, p.value);
    TEST_ASSERT_EQUAL_INT(1, p.valueChanged);
    TEST_ASSERT_EQUAL_INT(EndlessPotentiometer::CLOCKWISE, p.direction);
    TEST_ASSERT_TRUE(p.isMoving);
}

void test_360_degrees_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

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
    RUN_TEST(test_90_degrees_ccw);
    RUN_TEST(test_90_degrees_cw);
    RUN_TEST(test_180_degrees_ccw);
    RUN_TEST(test_180_degrees_cw);
    RUN_TEST(test_270_degrees_ccw);
    RUN_TEST(test_270_degrees_cw);
    RUN_TEST(test_360_degrees_ccw);
    RUN_TEST(test_360_degrees_cw);
    UNITY_END();
}