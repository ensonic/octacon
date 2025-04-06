#include <unity.h>
#include <math.h>

#include "EndlessPotentiometer.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

const float MAX_ADC_VALUE=1.0;
const float MID_ADC_VALUE=0.0;
const float MIN_ADC_VALUE=-1.0;
const float V0 = 0.0;
const float step = 1.0/360.0; // 1°

void init_pot(EndlessPotentiometer &p) {
    // start from a defined position
    p.update(MID_ADC_VALUE, MAX_ADC_VALUE);
}

void test_no_movement() {
    EndlessPotentiometer p;
    init_pot(p);

    float v1 = p.update(MID_ADC_VALUE, MAX_ADC_VALUE);
    float v2 = p.update(MID_ADC_VALUE, MAX_ADC_VALUE);
    TEST_ASSERT_EQUAL_FLOAT(V0, v2);
    TEST_ASSERT_EQUAL_FLOAT(v1, v2);
}

void test_move_ccw() {
    EndlessPotentiometer p;
    init_pot(p);

    float v1 = p.update(MID_ADC_VALUE, MAX_ADC_VALUE);
    float v2 = p.update(MID_ADC_VALUE-step, MAX_ADC_VALUE-step);
    TEST_ASSERT_NOT_EQUAL_FLOAT(0.0, v2);
    TEST_ASSERT_LESS_THAN_FLOAT(v1, v2); // v2 < v1
}

void test_move_cw() {
    EndlessPotentiometer p;
    init_pot(p);

    float v1 = p.update(MID_ADC_VALUE, MAX_ADC_VALUE);
    float v2 = p.update(MID_ADC_VALUE+step, MAX_ADC_VALUE-step);
    TEST_ASSERT_NOT_EQUAL_FLOAT(0.0, v2);
    TEST_ASSERT_GREATER_THAN_FLOAT(v1, v2); // v2 > v1
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_no_movement);
    RUN_TEST(test_move_ccw);
    RUN_TEST(test_move_cw);
    UNITY_END();
}
