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

#ifndef FLOWSENSOR_H_ //CLOCK_H will only be defined once
#define FLOWSENSOR_H_

#define SENSOR  PORTC, 4    // WT0CCP0 timer, counts rotations/edges

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initFlow();
uint32_t getFlowRaw(uint8_t secWait);
float getFlowFromRaw(uint32_t raw);
float getFlowGpm(uint8_t secWait);

#endif
