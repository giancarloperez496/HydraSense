#include "sensors.h"
#include "timer.h"
#include <stdlib.h>

void initSensors() {
    seed_rng();
}
uint8_t readValvePosition() {
    return rand32() % 2;
}

float readFlowMeter() {
    float f = rand32();
    float percent = f/(float)0x7FFFFFFF;
    float result = 5.00f * percent;
    return result;
}

float readPressure() {
    float f = rand32();
    float percent = f/(float)0x7FFFFFFF;
    float result = 5.00f * percent;
    return result;
}
