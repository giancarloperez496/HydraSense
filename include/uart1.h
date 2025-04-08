// UART0 Library
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef UART1_H_
#define UART1_H_

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
typedef void (*uart_callback)(char* str);
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

inline void enableUart1RxInt(void);
inline void disableUart1RxInt(void);
void setUart1Callback(uart_callback fn);
void initUart1(void);
void setUart1BaudRate(uint32_t baudRate, uint32_t fcyc);
void putcUart1(char c);
void putsUart1(const char* str);
void putcUart1_int(char c);
void putsUart1_int(const char* str);
char getcUart1(void);
bool isUart1RxFifoEmpty(void);
void uart1Isr();


#endif
