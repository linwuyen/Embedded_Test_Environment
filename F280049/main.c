//#include <stdint.h>
#include "common.h"

#define  DSP_FW_Version_Code        0x0805  //DSP ver

ST_DRV sDrv = {
        .fgStatus = _CSTAT_INIT_DRV_PARAM,
        .f32A = 1.123456f,
        .f32B = 3.654853f,
        .f32C = 9.876543f,
        .f32D = 0.123465f,
        .f32E = 11.22334f
};


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
    CLA_Module_Init();      // Initialize CLA module
    Diag_Init();            // Initialize diagnostic module
    WaveGen_Init();         // Initialize waveform generator manager (DDS + SPWM)

    // Set default mode and parameters
    WaveGen_SetMode(WAVEGEN_MODE_SPWM);  // Start with SPWM mode
    WaveGen_SetFrequency(60.0f);         // 60 Hz
    WaveGen_SetAmplitude(1.65f);         // 1.65V
    WaveGen_Enable(0);                   // Start disabled

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
       pollTimeTask();

//       runManualFlashApi();
//       runFlashStorage();
    }
}


//
// End of File
//
