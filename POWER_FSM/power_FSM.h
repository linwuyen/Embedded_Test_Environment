#ifndef POWER_FSM_H
#define POWER_FSM_H

#include <stdint.h>
#include <stdbool.h>

// --- 1. 狀態定義 (States) ---
typedef enum {
    PS_IDLE,        // 待機 (PWM Off)
    PS_CHECK_INIT,  // 開機前檢查 (例如：檢查 Vin 是否足夠)
    PS_SOFT_START,  // 軟啟動 (Ramping Reference)
    PS_RUN,         // 正常運作 (Closed Loop Regulation)
    PS_FAULT,       // 故障鎖定 (Protection Latch)
} PowerState_t;

// --- 2. 事件定義 (Events) ---
typedef enum {
    EVT_NONE = 0,
    EVT_CMD_ON,     // 收到開機指令
    EVT_CMD_OFF,    // 收到關機指令
    EVT_FAULT_TRIP, // 硬體觸發保護 (OVP, OCP)
    EVT_CLR_FAULT   // 清除故障指令
} PowerEvent_t;

// --- 3. 硬體操作介面 (Hardware Abstraction Layer) ---
// 這是「低耦合」的關鍵。FSM 不知道怎麼開 PWM，它只呼叫這些函數。
// 你需要在 main.c 裡實作這些函數。
typedef struct {
    // 讀取輸入電壓 (return true if normal)
    bool (*check_input_voltage)(void);
    
    // 開啟/關閉 PWM
    void (*enable_pwm_output)(bool enable);
    
    // 設定參考電壓 (用於軟啟動, 0.0 ~ 1.0 代表 0% ~ 100%)
    void (*set_ref_voltage)(float ref_ratio);
    
    // 取得目前是否有硬體故障 (回傳故障碼，0 代表無故障)
    uint16_t (*get_hw_fault_status)(void);
    
} PowerOps;

// --- 4. 狀態機物件 (Context) ---
typedef struct {
    // 內部變數 (Private)
    PowerState_t current_state;
    PowerState_t prev_state;
    uint32_t     state_timer;      // 通用計時器 (Tick count)
    float        soft_start_ratio; // 軟啟動進度 (0.0 ~ 1.0)
    uint16_t     fault_code;       // 故障碼
    
    // 參數設定 (Config)
    uint32_t     ss_duration_ticks;// 軟啟動需要幾個 Tick
    
    // 外部掛勾 (Hooks)
    const PowerOps *ops;           // 指向硬體操作函數的指標

} PowerFSM;

// --- API ---
void FSM_Init(PowerFSM *fsm, const PowerOps *ops);
void FSM_HandleEvent(PowerFSM *fsm, PowerEvent_t evt); // 處理外部命令
void FSM_RunTick(PowerFSM *fsm); // 週期性執行 (放在 1ms Loop 或 Background)

#endif // POWER_FSM_H