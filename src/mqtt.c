/*
 * mqtt.c
 *
 *  Created on: Mar 22, 2025
 *      Author: gianc
 */

#include "mqtt.h"
#include "sim7600.h"
#include "strlib.h"
#include "uart1.h"
#include "uart0.h"
#include "wait.h"
#include "shell.h"
#include <stdint.h>

#define MQTTBROKER "99.73.216.217"
#define MQTTPORT "1883"

#define MQTTUSER "tm4c"
#define MQTTPASS "kr-xh-ps"

#define MQTT_NOTREADY 0
#define MQTT_STARTING 1
#define MQTT_ACQUIRING_CLIENT 2
#define MQTT_CONNECTING 3
#define MQTT_CONNECTED 4
#define MQTT_SETTING_TOPIC 5
#define MQTT_SETTING_PAYLOAD 6
#define MQTT_PUBLISHING 7

static uint8_t mqttState = 0;
static char publishingData[50];
static char dataLen[5];

void startMqttClient() {
    sendATCommand("AT+CMQTTSTART");
}

void mqttAcquireClient(const char* cid) {
    printShell("AT+CMQTTACCQ=0,\"");
    putsUart0(cid);
    putsUart0("\"\n");

    putsUart1("AT+CMQTTACCQ=0,\"");
    putsUart1(cid);
    putsUart1("\"\r");
}

void mqttConnect(char* server) {
    printShell("AT+CMQTTCONNECT=0,\"tcp://");
    putsUart0(server);
    putcUart0(':');
    putsUart0(MQTTPORT);
    putsUart0("\",60,1,\"");
    putsUart0(MQTTUSER);
    putsUart0("\",\"");
    putsUart0(MQTTPASS);
    putsUart0("\"\n");

    putsUart1("AT+CMQTTCONNECT=0,\"tcp://");
    putsUart1(server);
    putcUart1(':');
    putsUart1(MQTTPORT);
    putsUart1("\",60,1,\"");
    putsUart1(MQTTUSER);
    putsUart1("\",\"");
    putsUart1(MQTTPASS);
    putsUart1("\"\r");
}

void mqttPublish(const char* topic, char* data, uint8_t len) {
    if (mqttState != 4) {
        return;
    }
    uint32_t topicLen = str_length(topic);
    char topicLenStr[5];
    to_string(topicLen, topicLenStr, 10);
    str_copy(publishingData, data);
    to_string(len, dataLen, 10);

    printShell("AT+CMQTTTOPIC=0,");
    putsUart0(topicLenStr);
    putcUart0('\n');

    putsUart1("AT+CMQTTTOPIC=0,");
    putsUart1(topicLenStr);
    putcUart1('\r');

    waitMicrosecond(10000);
    mqttState = 5;

    printShell("> ");
    putsUart0(topic);
    putcUart0('\n');

    putsUart1(topic);
    //putcUart1(0x1A);
}

uint8_t getMqttState() {
    return mqttState;
}

void processMqttResponse(char* resp) {
    switch (mqttState) {
    case MQTT_NOTREADY:
        startMqttClient();
        mqttState = 1;
        break;
    case MQTT_STARTING: //waiting for OK from MQTTSTART
        if (str_equal(resp, "+CMQTTSTART: 0")) {
            mqttAcquireClient("TM4C");
            mqttState = 2;
        }
        break;
    case MQTT_ACQUIRING_CLIENT: //network opened, acquire client
        if (str_equal(resp, "OK")) {// received OK from client acquire
            mqttConnect(MQTTBROKER);
            mqttState = 3;
        }
        break;
    case MQTT_CONNECTING: //waiting for OK from connect.
        if (str_equal(resp, "+CMQTTCONNECT: 0,0")) {// received OK from client acquire
            printShell("Connected to MQTT Broker: ");
            putsUart0(MQTTBROKER);
            putcUart0(':');
            putsUart0(MQTTPORT);
            putcUart0('\n');
            mqttState = 4;
        }
        else if (str_equal(resp, "ERROR")) {
            //start timer to retry
        }
        else {
            //connection rejected.
        }
        break;
    case MQTT_CONNECTED: //connected state.

        break;
    case MQTT_SETTING_TOPIC: //setting topic
        if (str_equal(resp, "OK")) {
            //received OK for mqtt topic command, set payload next.
            putsUart0("AT+CMQTTPAYLOAD=0,");
            putsUart0(dataLen);
            putcUart0('\n');

            putsUart1("AT+CMQTTPAYLOAD=0,");
            putsUart1(dataLen);
            putcUart1('\r');

            waitMicrosecond(10000);

            printShell("> ");
            putsUart0(publishingData);
            putcUart0('\n');

            putsUart1(publishingData);
            //putcUart1(0x1A); //CTRL Z
            mqttState = 6;
        }
        break;
    case MQTT_SETTING_PAYLOAD:
        if (str_equal(resp, "OK")) { //if received OK from setting payload data, publish
            putsUart1("AT+CMQTTPUB=0,2,60\r");
            mqttState = 7;
        }
        break;
    case MQTT_PUBLISHING:
        if (str_equal(resp, "OK")) { //received OK from publish, data sent.
            mqttState = 4; // go back to state 4;
        }
        break;
    }
}

void mqttHandler() {
    switch (mqttState) {
    case MQTT_NOTREADY:
        startMqttClient();
        mqttState = 1;
        break;
    case MQTT_STARTING: //waiting for OK //add timeout to see if its waiting too long, same for other states.
        break;
    case MQTT_ACQUIRING_CLIENT: //network opened, acquire client, waiting for OK from ACCQ
        break;
    case MQTT_CONNECTING: //waiting for OK from connect.
        break;
    case MQTT_CONNECTED: //connected state.
        break;
    }
}
