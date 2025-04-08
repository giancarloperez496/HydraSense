#ifndef MQTT_H_
#define MQTT_H_

#define MQTT_NOTRUNNING 0
#define MQTT_STARTING 1
#define MQTT_ 2

void startMqttClient();
void mqttAcquireClient(const char* cid);
void mqttConnect();
void mqttHandler();
void processMqttResponse(char* resp);

#endif /* MQTT_H_ */
