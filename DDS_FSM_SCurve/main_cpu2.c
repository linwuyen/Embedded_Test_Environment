/**
 * @file main_cpu2.c
 * @brief CPU2 主程式 (High-Speed Calculation Engine)
 */
#include "driverlib.h"
#include "device.h"
#include "dds.h"  // All-in-One DDS Header

// Shared Memory Definition
#pragma DATA_SECTION(g_DDS_Shared, "SHARERAMGS0");
volatile DDS_IPC_Data_t g_DDS_Shared;

DDS_Handle_t dds_local;
uint16_t last_seq_id = 0;

// Flash-to-RAM copy variables
extern uint16_t RamfuncsLoadStart;
extern uint16_t RamfuncsLoadEnd;
extern uint16_t RamfuncsLoadSize;
extern uint16_t RamfuncsRunStart;
extern uint16_t RamfuncsRunEnd;
extern uint16_t RamfuncsRunSize;

__interrupt void epwm1_isr(void);

void main(void) {
    // 1. System Initialization
    Device_init(); 
    Interrupt_initModule(); 
    Interrupt_initVectorTable();

#ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif

    // 2. DDS Initialization
    DDS_Global_Init(); 
    DDS_InitObj(&dds_local, DDS_ISR_FREQ_HZ);
    
    // 3. PWM Initialization
    // 假設系統已由 CPU1 設定好時脈，CPU2 僅需設定 ePWM
    EPWM_setInterruptSource(EPWM1_BASE, EPWM_INT_TBCTR_ZERO);
    EPWM_enableInterrupt(EPWM1_BASE); 
    EPWM_setInterruptEventCount(EPWM1_BASE, 1); 
    Interrupt_register(INT_EPWM1, &epwm1_isr); 
    Interrupt_enable(INT_EPWM1);
    
    EINT; ERTM;
    
    // 4. Idle Loop
    while(1) {}
}

__interrupt void epwm1_isr(void) {
    // [Safe IPC] 使用 Fetch API 安全地取得資料
    DDS_Transfer_t ipc_data;
    if (DDS_IPC_Fetch(&g_DDS_Shared, &ipc_data, &last_seq_id)) {
        // 資料更新：批量套用到核心
        DDS_ApplySettings(&dds_local, &ipc_data);
    }

    float dds_out = 0.0f;
    // 軟停止邏輯：若 v_set 已經是 0，這裡就會輸出 0
    dds_out = DDS_Update(&dds_local);
    
    // 轉換為 Duty Cycle (雙極性調變: +1 -> 98%, -1 -> 2%)
    float duty = (dds_out + 1.0f) * 0.5f;
    if(duty > 0.98f) duty = 0.98f; 
    if(duty < 0.02f) duty = 0.02f;
    
    uint16_t cmp = (uint16_t)(duty * (float)DDS_PWM_TBPRD_VAL);
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, cmp);
    
    EPWM_clearEventTriggerInterruptFlag(EPWM1_BASE); 
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}