#include "sim7600.h"
#include "shell.h"
#include "gpio.h"
#include "uart0.h"
#include "uart1.h"
#include "nvic.h"
#include "mqtt.h"
#include "strlib.h"
#include "wait.h"
#include "tm4c123gh6pm.h"

#define RST PORTA,7

#define NOTREADY 0
#define READY 1
#define RESETTING 2

uint8_t moduleState = 0;
char lastResponse[64] = "";

void cleanResponse(char* str) { //gets rid of newlines/RC
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] != '\r' && str[i] != '\n') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

#define SIM_INITIALIZING 0
#define SIM_DISABLING_ECHO 1
#define SIM_CHECKING_SIM 2
#define SIM_RUNNING 3


void processATResponse(char* resp) {
    switch (moduleState) { //debug switch to set breakpoints before cleaning the response on certain states(not all)
        case 0:
            break;
        case 1:
            break;
    }

    cleanResponse(resp);
    if (resp[0] == 0) { // if empty response
        return;
    }

    putsUart0("[SIM7600]: ");
    putsUart0(resp);
    putsUart0("\n");

    switch (moduleState) {
    case SIM_INITIALIZING: //initializing
        if (str_equal(resp, "PB DONE")) {
            sendATCommand("ATE0"); //received RDY, send AT
            moduleState = 1;
        }
        break;
    case SIM_DISABLING_ECHO:
        if (str_equal(resp, "ATE0")) { //received echo from ATE0
            sendATCommand("AT+CPIN?");
            moduleState = 2;
        }
        break;
    case SIM_CHECKING_SIM:
        if (str_equal(resp, "+CPIN: READY")) { //received OK from AT
            moduleState = 3;
        }
    case SIM_RUNNING:
        processMqttResponse(resp);
        break;
    }
}

void retryInitCallback() {
    moduleState = 1;
}

void resetSim7600() {
    sendATCommand("AT+CRESET");
    //turning off
    setPinValue(RST, 0);
    waitMicrosecond(5e6);
    setPinValue(RST, 1);
    //wait
    waitMicrosecond(2e6);
    //turning on
    setPinValue(RST, 0);
    waitMicrosecond(3e6);
    setPinValue(RST, 1);
    moduleState = 0;
}

void sendATCommand(char* cmd) {
    putsUart0("[TM4C]: ");
    putsUart0(cmd);
    putsUart0("\n");

    putsUart1(cmd);
    putcUart1('\r');
}


void sim7600Handler() {
    switch (moduleState) {
    case SIM_INITIALIZING:
        break;
    case SIM_DISABLING_ECHO:
        break;
    case SIM_CHECKING_SIM:
        break;
    case SIM_RUNNING:
        mqttHandler(); //call mqtt state machine
        break;
    }
}

void initSim7600() {
    initUart1();
    setUart1BaudRate(115200, 40e6);
    setUart1Callback(processATResponse);
    enableUart1RxInt();
    enableNvicInterrupt(INT_UART1);

    enablePort(PORTA);
    selectPinPushPullOutput(RST);

    printShell("Initializing UART1 (SIM7600A-H)\n");
    resetSim7600();

}
