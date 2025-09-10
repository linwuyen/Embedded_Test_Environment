
//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"



uint16_t ADC_soft;
uint16_t PWMCNT;

void emu_ADC(void){

    ADC_forceSOC(myADC0_BASE,ADC_SOC_NUMBER0);

//    while(ADC_getInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1) == false)
//    {
//        // 持續空轉等待...
//    }
//    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);

    ADC_soft = ADC_readResult(myADC0_RESULT_BASE,ADC_SOC_NUMBER0);

;
}
//

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
        emu_ADC();

        PWMCNT = EPWM_getTimeBaseCounterValue(myEPWM0_BASE);
    }
}



// End of File
//
