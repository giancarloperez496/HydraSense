// Position Sensor Library
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

#ifndef POSITION_H_ //CLOCK_H will only be defined once
#define POSITION_H_

#define REED PORTA, 2

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initPosition();
bool getPositionRaw();

#endif
