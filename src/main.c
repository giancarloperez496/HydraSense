#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "uart0.h"
#include "uart1.h"
#include "wait.h"
#include "sim7600.h"
#include "shell.h"
#include "sensors.h"
#include "gpio.h"
#include "strlib.h"
#include "nvic.h"


void periodicCallback() {
    publishSensors();
}

void publishSensors() {
    uint8_t state = getMqttState();
    if (state != 4) {
        return;
    }
    uint8_t v = readValvePosition();
    float f = readFlowMeter();
    float p = readPressure();
    char pubBuffer[50]; //50 is max publish length
    uint8_t len = snprintf(pubBuffer, 50, "%d,%.2f,%.2f", v, f, p);
    printShell("Publishing data: \"");
    putsUart0(pubBuffer);
    putsUart0("\" - Length: ");
    char lenbuf[5];
    to_string(len, lenbuf, 10);
    putsUart0(lenbuf);
    putcUart0('\n');
    mqttPublish("sensors", pubBuffer, len);
}

int main(void) {
    initSystemClockTo40Mhz();
    initTimer();
    initShell();
    initSim7600();
    startPeriodicTimer(periodicCallback, 60, 0); //publish every 2 minutes
    while (1) {
        sim7600Handler();
    }
}
