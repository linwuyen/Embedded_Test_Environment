
//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"

#include "lut.h"

//
// Main
//
void main(void)
{

    //1213
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
    // Initialize DDS Table
    //
    Init_DDS_Table();

    //
    // Set Initial Frequency (e.g. 60Hz)
    //
    Update_Frequency(60.0f);

    //
    // Enable Global Interrupt (INTM) and real time interrupt (DBGM)
    //
    EINT;
    ERTM;

    //
    // [Diagnostic] Configure GPIO16 AND GPIO18 as XCLKOUT
    // OUTPUT: 12.5 MHz
    //
//    GPIO_setPinConfig(GPIO_16_XCLKOUT);
//    GPIO_setDirectionMode(16, GPIO_DIR_MODE_OUT);
//    GPIO_setPadConfig(16, GPIO_PIN_TYPE_STD);
//
//    GPIO_setPinConfig(GPIO_18_XCLKOUT);
//    GPIO_setDirectionMode(18, GPIO_DIR_MODE_OUT);
//    GPIO_setPadConfig(18, GPIO_PIN_TYPE_STD);
//
//    SysCtl_selectClockOutSource(SYSCTL_CLOCKOUT_SYSCLK);
//    SysCtl_setXClk(SYSCTL_XCLKOUT_DIV_8);

    while(1)
    {

    }
}

//
// DDS ISR
//
__interrupt void INT_myEPWM0_ISR(void)
{
//    // 1. [Atomic Sync] 從 Shadow 載入新的步進值
//    g_FreqStep_Active = g_FreqStep;
//
//    // 2. [Accumulate] 32-bit 整數累加
//    g_PhaseAcc.u32Accumulator += g_FreqStep_Active;
//
//    // 3. [Lookup] 利用結構的 Bit-field 直接拿 Index
//    // (g_PhaseAcc.bit.u32Index 對應高 12 bits)
//    uint16_t idx = g_PhaseAcc.bit.u32Index;
//
//    // 4. [Output] 查表 (Fixed Point: 直接讀取 0-4095)
//    uint16_t dacVal = SineTable[idx];
//
//    // 寫入 DAC Shadow
//    DAC_setShadowValue(myDAC0_BASE, dacVal);

    // [Diagnostic] Output 60kHz Clock (ISR Frequency / 2)
    // 透過已知良好的 GPIO0 驗證時鐘誤差
    // 理論值: 60.000 kHz
    // 若晶振快 400ppm: 量測值將為 60.024 kHz
    GPIO_togglePin(myGPIO0);

    //
    // Clear the interrupt flag
    //
    EPWM_clearEventTriggerInterruptFlag(myEPWM0_BASE);

    //
    // Acknowledge the interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}


//
// End of File
//
