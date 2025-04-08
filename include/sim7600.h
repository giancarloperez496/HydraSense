#ifndef SIM7600_H_
#define SIM7600_H_

#include <stdbool.h>

void initSim7600();
void sendATCommand(char* cmd);
inline bool isLastResponse(char* str);

#endif
