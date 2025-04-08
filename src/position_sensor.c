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

#include <stdint.h>
#include "clock.h"
#include "gpio.h"
#include "position_sensor.h"
#include "tm4c123gh6pm.h"
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initPosition()
{
    // reed switch connected as pull down and connected to 5V
     enablePort(PORTA);
     selectPinDigitalInput(REED);
     enablePinPulldown(REED);
}

// returns boolean open(1) or closed(0)
bool getPositionRaw()
{
    return getPinValue(REED);
}
