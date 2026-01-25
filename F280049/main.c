//#include <stdint.h>
#include "common.h"

#define  DSP_FW_Version_Code        0x0805  //DSP ver

// sDrv definition moved to c28param.c

uint16_t cc;
uint16_t x;

//
// Main
//
void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    // The fapi_ram_LoadStart, fapi_ram_LoadSize, and fapi_ram_RunStart symbols
    // are created by the linker. Refer to the device .cmd file.
    //
    memcpy(&fapi_ram_RunStart, &fapi_ram_LoadStart, (size_t)&fapi_ram_LoadSize);

    // FLASH Initialization:
    // The "FLASH_init()" should be called after or during initialization functions like
    // Device_init() or Device_enableAllPeripherals().
//    FLASH_init();


    //
    // Initialize GPIO and configure the GPIO pin as a push-pull output
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
    // Initialize SysConfig Configuration
    //
    Board_init();
    init_EPWM();

    //
    // Initialize New Integrated Modules
    //
    ADC_Module_Init();      // Initialize ADC module
    // CLA_Module_Init();      // Initialize CLA module - TODO: Add cla_tasks.cla to CCS project build
//    Diag_Init();            // Initialize diagnostic module
////    WaveGen_Init();         // Initialize waveform generator manager (DDS + SPWM)
//
//    // Set default mode and parameters
//    WaveGen_SetMode(WAVEGEN_MODE_DDS);   // Use DDS mode for high precision
//    WaveGen_SetFrequency(60.0f);         // 60 Hz
//    WaveGen_SetAmplitude(1.65f);         // 1.65V
//    // Register DDS ISR to EPWM1 interrupt
//    //
//    extern __interrupt void INT_DDS_ISR(void);
//    Interrupt_register(INT_EPWM1, &INT_DDS_ISR);
//    Interrupt_enable(INT_EPWM1);
//    WaveGen_Enable(1);                   // ⭐ Enable DDS output
//


    //
    // Force enable ADC interrupt (independent of SPI mode)
    //
    // Force enable ADC interrupt (independent of SPI mode)
    //
    Interrupt_register(INT_ADCA1, &INT_CV_AD_1_ISR); // ⭐ Explicitly register
    Interrupt_enable(INT_ADCA1);

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    //
    // Loop Forever
    //
    for(;;)
    {
       // Time task polling (includes ADC processing)
//       pollTimeTask();
        DAC_setShadowValue(myDACB_BASE,x);
//       runManualFlashApi();
//       runFlashStorage();
    }
}

// Simplified ADC ISR - DISABLED (focusing on DDS only)
// PWM 200kHz trigger
__interrupt void INT_CV_AD_1_ISR(void){
//
//
//
//
//    // Clear interrupt flags only
    ADC_clearInterruptStatus(CV_AD_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INT_CV_AD_1_INTERRUPT_ACK_GROUP);
}

__interrupt void  INT_DDS_1_ISR (void){
    // ADC functionality disabled - focusing on DDS output
    DDS_Task();

    static uint16_t cc = 0;

    cc++;
    // Clear interrupt flags only
    ADC_clearInterruptStatus(DDS_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INT_DDS_1_INTERRUPT_ACK_GROUP);
}

//
// End of File
//
