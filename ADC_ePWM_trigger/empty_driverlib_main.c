
//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"



uint16_t ADC_soft;
uint16_t PWMCNT;

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

//        ADC_forceSOC(myADC0_BASE,ADC_SOC_NUMBER0);
//        ADC_soft = ADC_readResult(myADC0_RESULT_BASE,ADC_SOC_NUMBER0);
        PWMCNT = EPWM_getTimeBaseCounterValue(myEPWM0_BASE);
    }
}


__interrupt void INT_myADC0_1_ISR (void){
    ADC_soft = ADC_readResult(myADC0_RESULT_BASE,ADC_SOC_NUMBER0);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);

    //
    // Check if overflow has occurred
    //
    if(true == ADC_getInterruptOverflowStatus(myADC0_BASE, ADC_INT_NUMBER1))
    {
        ADC_clearInterruptOverflowStatus(myADC0_BASE, ADC_INT_NUMBER1);
        ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    }

    //
    // Acknowledge the interrupt
    //
    Interrupt_clearACKGroup(INT_myADC0_1_INTERRUPT_ACK_GROUP);
}

// End of File
//
