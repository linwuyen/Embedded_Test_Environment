/**
 * @file main_cpu1.c
 * @brief CPU1 主程式 (High-Level Controller)
 */
#include "driverlib.h"
#include "device.h"
#include "dds.h"  // All-in-One DDS Header     

// Shared Memory Definition
#pragma DATA_SECTION(g_DDS_Shared, "SHARERAMGS0");
volatile DDS_IPC_Data_t g_DDS_Shared;

// Flash-to-RAM copy variables
extern uint16_t RamfuncsLoadStart;
extern uint16_t RamfuncsLoadEnd;
extern uint16_t RamfuncsLoadSize;
extern uint16_t RamfuncsRunStart;
extern uint16_t RamfuncsRunEnd;
extern uint16_t RamfuncsRunSize;

// [NEW] CPU1 System Tick ISR (1ms)
__interrupt void on_cpu_timer0_isr(void) {
    DDS_Service_Tick(); 
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

void main(void) {
    // 1. System Initialization
    Device_init(); 
    Device_initGPIO(); 
    Interrupt_initModule(); 
    Interrupt_initVectorTable();

#ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif

    // [NEW] Register Timer0 ISR
    Interrupt_register(INT_TIMER0, &on_cpu_timer0_isr);

    // [NEW] Initialize CPUTimer0 to 1ms (Assuming 200MHz CPU)
    // 200MHz, 1000us period
    CPUTimer_setPeriod(CPUTIMER0_BASE, 200000000 / 1000); 
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0); 
    CPUTimer_stopTimer(CPUTIMER0_BASE); 
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE); 
    CPUTimer_startTimer(CPUTIMER0_BASE);
    
    // Enable Timer0 Interrupt
    Interrupt_enable(INT_TIMER0);

    // 2. Initialize Shared Memory (Before booting CPU2)
    memset((void*)&g_DDS_Shared, 0, sizeof(g_DDS_Shared));
    g_DDS_Shared.seq_id = 0;
    
    // 3. Boot CPU2
#ifdef _FLASH
    IPC_bootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
#else
    IPC_bootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_RAM);
#endif

    // [Optimization] DDS Init on CPU1 is only for Service internal state. 
    // It does not need to compute waveforms.
    DDS_Service_Init(); 

    EINT; ERTM;

    // 4. Main Loop
    while(1) {
        // [Task 1] 執行應用層邏輯 (Sequencer, Ramping)
        // Now driven by Tick ISR -> Flag -> Background
        DDS_Service_Run_Background(); 
        
        // [Task 2] 閉迴路控制 (Stub)
        // DDS_Service_UpdateClosedLoop(v_adc_in);
        
        // [Task 3] 同步資料 (Decoupled Bridge)
        DDS_Transfer_t data_transfer;
        DDS_Service_GetData(&data_transfer);
        
        DDS_IPC_Commit(&g_DDS_Shared, &data_transfer);
    }
}