//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"


#pragma DATA_SECTION(fVal,"CpuToCla1MsgRAM");
float fVal;

#pragma DATA_SECTION(fResult,"Cla1ToCpuMsgRAM");
float fResult;

#pragma DATA_SECTION(u16Result,"Cla1ToCpuMsgRAM");
uint16_t u16Result;

//
// Main
//
void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Board_init();

	 for(;;)
	    {
	     //software Trigger
	     CLA_forceTasks(CLA1_BASE,CLA_TASKFLAG_1);
	    }
}


