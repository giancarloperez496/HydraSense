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

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "gpio.h"
#include "ring_buffer.h"

// Pins
#define UART0_TX PORTA,1	// transmit
#define UART0_RX PORTA,0	// receive
#define UART0_BUFFER_SIZE (64)

typedef void (*uart_callback)(char* str);
static uart_callback uart0LnCallback = 0;
static ring_buffer tx_buffer = { .size = UART0_BUFFER_SIZE, .head = 0, .tail = 0 };
static ring_buffer rx_buffer = { .size = UART0_BUFFER_SIZE, .head = 0, .tail = 0 };

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

static inline void clearUart0TxInt(void) {
    UART0_ICR_R |= UART_ICR_TXIC;
}

static inline void clearUart0RxInt(void) {
    UART0_ICR_R |= UART_ICR_RXIC;
}

static inline void enableUart0TxInt(void) { // MUST BE ENABLED WHEN READY TO TRANSMIT
    UART0_IM_R |= UART_IM_TXIM;
}

static inline void disableUart0TxInt(void) { //ONCE TX BUFFER EMPTY, DISABLE INTERRUPT
    UART0_IM_R &= ~UART_IM_TXIM;
}

inline void enableUart0RxInt(void) {
    UART0_IM_R |= UART_IM_RXIM;
}

inline void disableUart0RxInt(void) {
    UART0_IM_R &= ~UART_IM_RXIM;
}

void setUart0Callback(uart_callback fn) { //function to callback when a line is entered into the UART
    uart0LnCallback = fn;
}

// Initialize UART0
void initUart0(void) {
    // Enable clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    _delay_cycles(3);
    enablePort(PORTA);

    // Configure UART0 pins
    selectPinPushPullOutput(UART0_TX);
    selectPinDigitalInput(UART0_RX);
    setPinAuxFunction(UART0_TX, GPIO_PCTL_PA1_U0TX);
    setPinAuxFunction(UART0_RX, GPIO_PCTL_PA0_U0RX);	// set function for the receive pin

    // Configure UART0 with default baud rate
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_CC_R = UART_CC_CS_SYSCLK;                     // use system clock (usually 40 MHz)
}

// Set baud rate as function of instruction cycle frequency
void setUart0BaudRate(uint32_t baudRate, uint32_t fcyc) {
    uint32_t divisorTimes128 = (fcyc * 8) / baudRate;   // calculate divisor (r) in units of 1/128,
                                                        // where r = fcyc / 16 * baudRate
    divisorTimes128 += 1;                               // add 1/128 to allow rounding
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_IBRD_R = divisorTimes128 >> 7;                // set integer value to floor(r)
    UART0_FBRD_R = ((divisorTimes128) >> 1) & 63;       // set fractional value to round(fract(r)*64)
    UART0_LCRH_R = UART_LCRH_WLEN_8;// | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO
    //UART0_IFLS_R = UART_IFLS_RX1_8 | UART_IFLS_TX4_8; // Trigger ISR on RX FIFO >= 1/8 and TX FIFO <= 1/2
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN; // turn-on UART0
}

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c) {
    while (UART0_FR_R & UART_FR_TXFF);               // wait if uart0 tx fifo full
    UART0_DR_R = c;                                  // write character to fifo
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(const char* str) {
    uint8_t i = 0;
    while (str[i] != '\0')
        putcUart0(str[i++]);
}

// Interrupt-driven function that adds character to buffer to be transmitted when TX FIFO not full
void putcUart0_int(char c) {
    uint8_t next = (tx_buffer.head + 1) % UART0_BUFFER_SIZE;
    //while (next == tx_buffer.tail);
    if (next == tx_buffer.tail) return;
    tx_buffer.buffer[tx_buffer.head] = c; // insert the character at the head
    tx_buffer.head = next; //increment the head and loop around if needed
    enableUart0TxInt();
}

void putsUart0_int(const char* str) {
    while (*str) {
        putcUart0_int(*str++);
    }
}

// Blocking function that returns with serial data once the buffer is not empty
char getcUart0(void) {
    while (UART0_FR_R & UART_FR_RXFE);               // wait if uart0 rx fifo empty
    return UART0_DR_R & 0xFF;                        // get character from fifo
}

// Returns the status of the receive buffer
bool isUart0RxFifoEmpty(void) {
    return UART0_FR_R & UART_FR_RXFE;
}

void uart0Isr() {
    if (UART0_MIS_R & UART_MIS_TXMIS) {
        while ((~UART0_FR_R & UART_FR_TXFF) && (tx_buffer.head != tx_buffer.tail)) {
            UART0_DR_R = tx_buffer.buffer[tx_buffer.tail];
            tx_buffer.tail = (tx_buffer.tail + 1) % UART0_BUFFER_SIZE;
        }
        if (tx_buffer.head == tx_buffer.tail) {
            disableUart0TxInt();
        }
        clearUart0TxInt();
    }
    if (UART0_MIS_R & UART_MIS_RXMIS) {
        while (~UART0_FR_R & UART_FR_RXFE) { //while FIFO is not empty
            char c = UART0_DR_R & 0xFF; //read next char in the FIFO
            uint8_t next = (rx_buffer.head + 1) % UART0_BUFFER_SIZE;
            if (c == '\r') {
                rx_buffer.buffer[rx_buffer.head] = 0; //null terminate
                if (uart0LnCallback) {
                    uart0LnCallback((const char*)rx_buffer.buffer);
                }
                rx_buffer.head = rx_buffer.tail = 0;
            } //else, insert the character as normal and move the head
            else if (next != rx_buffer.tail) { //make sure that there is room in the buffer
                rx_buffer.buffer[rx_buffer.head] = c; //insert the char at the head
                rx_buffer.head = next; //increment the head
            }
        }
        clearUart0RxInt();
    }
}
