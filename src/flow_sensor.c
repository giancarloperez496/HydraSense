// Flow Sensor Library
// Jennifer Hernandez

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// 16 MHz external crystal oscillator

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include "clock.h"
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "flow_sensor.h"
#include "wait.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------


// initializes PC4 as WT0CCP0 timer, WT1A as Periodic Timer
void initFlow()
{
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R0 | SYSCTL_RCGCWTIMER_R1;    // clocks for WT0
    _delay_cycles(3);

    enablePort( PORTC );
    selectPinDigitalInput(SENSOR);
    setPinAuxFunction(SENSOR, 7);          // WT0CCP0 timer, counts neg edges on phototransistor

    // Configure WTimer 0A, counting neg edges on phototransistor, no ISR
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;                   // turn-off timer before reconfiguring
    WTIMER0_CFG_R = TIMER_CFG_16_BIT;                   // select it as 16 bit timer
    WTIMER0_TAMR_R = TIMER_TAMR_TAMR_CAP | TIMER_TAMR_TACDIR;         //count up, edge count
    WTIMER0_CTL_R |= TIMER_CTL_TAEVENT_NEG;             // count neg edge
    WTIMER0_TAV_R = 0;                                  // start at 0
    WTIMER0_CTL_R |= TIMER_CTL_TAEN;                    // enable timer

}

// returns the edges counted by WT0 in the flow sensor after secWait seconds
uint32_t getFlowRaw(uint8_t secWait)
{
    WTIMER0_TAV_R = 0;                  //reset WT0 edge count
    waitMicrosecond(1000000*secWait);           // wait x seconds
    return WTIMER0_TAV_R;
}

// return the calculated GPM value from getFlowRaw() results
float getFlowFromRaw(uint32_t raw)
{
    return (0.0618*raw) + 0.0328;
}

// return calculated GPM value from edge counts in secWait seconds
float getFlowGpm(uint8_t secWait)
{
    WTIMER0_TAV_R = 0;                  //reset WT0 edge count
    waitMicrosecond(1000000*secWait);           // wait x seconds
    return (0.0618*WTIMER0_TAV_R) + 0.0328;
}


