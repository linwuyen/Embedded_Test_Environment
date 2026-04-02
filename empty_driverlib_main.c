
//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"


uint16_t tempData;

#include "McBSP_module/mcbsp_spi.h"


//
// Main
//
void main(void)
{

    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pull-ups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // PinMux and Peripheral Initialization
    //
    Board_init();

    // 1. Initialize McBSP SPI Master core settings
    Init_McBSP_SPI_Master(MCBSPA_BASE);

    // 2. Initialize DMA Pipeline using CH1 (TX) and CH2 (RX)
    // TX is triggered by EPWM1SOCA, RX is triggered by McBspa REVT
    Init_DMA_CompensationLoop(MCBSPA_BASE);

    // 3. Initialize EPWM1 as heartbeat (100kHz SOCA & GPIO0 toggle)
    Init_EPWM1_SOCA();


    GPIO_setPinConfig(GPIO_73_XCLKOUT);
    SysCtl_selectClockOutSource(SYSCTL_CLOCKOUT_SYSCLK);
    SysCtl_setXClk(SYSCTL_XCLKOUT_DIV_8);

    //
    // C2000Ware Library initialization
    //
    C2000Ware_libraries_init();

    //
    // Enable Global Interrupt (INTM) and real time interrupt (DBGM)
    //
    EINT;
    ERTM;

    while(1)
    {
        tempData++;

  }
}

//
// End of File
//
