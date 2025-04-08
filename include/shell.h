#ifndef _SHELL_C
#define _SHELL_C

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define MAX_OUT 50
#define MAX_FIELDS      5

typedef struct _shell_cmd {
    char    buffer[MAX_OUT+1];
    uint8_t fieldCount;
    uint8_t fieldPosition[MAX_FIELDS];
    char    fieldType[MAX_FIELDS];
} shell_cmd;

/*
 * shell.c
 *
 *  Created on: Sep 6, 2024
 *      Author:
 */

//-----------------------------------------------------------------------------
// Device includes
//-----------------------------------------------------------------------------

#include "shell.h"
#include "uart0.h"
#include "nvic.h"
#include "clock.h"
#include "tm4c123gh6pm.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void initShell();
void readLine(shell_cmd* data);
void parseFields(shell_cmd* data);
char* getFieldString(shell_cmd* data, uint8_t fieldNumber);
int32_t getFieldInteger(shell_cmd* data, uint8_t fieldNumber);
uint32_t getFieldHexInteger(shell_cmd* data, uint8_t fieldNumber);
bool isCommand(shell_cmd* data, const char strCommand[], uint8_t minArguments);
void processShellCmd(char* line);
void processShell();
void printShell(const char* str);

#endif
