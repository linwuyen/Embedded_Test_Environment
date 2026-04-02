#include "power_fsm.h"
#include <stddef.h>

// 軟啟動步階大小 (每次 Tick 增加多少)
// 為了範例簡單，這裡假設線性增加
static void RunSoftStart(PowerFSM *fsm);

void FSM_Init(PowerFSM *fsm, const PowerOps *ops) {
    if (fsm == NULL || ops == NULL) return;

    fsm->current_state = PS_IDLE;
    fsm->prev_state = PS_IDLE;
    fsm->state_timer = 0;
    fsm->soft_start_ratio = 0.0f;
    fsm->fault_code = 0;
    fsm->ss_duration_ticks = 100; // 預設軟啟動 100 個 Ticks
    fsm->ops = ops;
}

// 處理突發事件 (Command, Fault)
void FSM_HandleEvent(PowerFSM *fsm, PowerEvent_t evt) {
    if (fsm == NULL) return;

    switch (evt) {
        case EVT_CMD_ON:
            if (fsm->current_state == PS_IDLE) {
                // 從 IDLE 轉入 CHECK
                fsm->current_state = PS_CHECK_INIT;
                fsm->state_timer = 0;
            }
            break;

        case EVT_CMD_OFF:
            // 任何狀態下 (除了 Fault) 收到 OFF，都應轉回 IDLE
            if (fsm->current_state != PS_FAULT) {
                fsm->current_state = PS_IDLE;
                // 立即關閉 PWM
                if (fsm->ops->enable_pwm_output) {
                    fsm->ops->enable_pwm_output(false);
                }
            }
            break;

        case EVT_FAULT_TRIP:
            // 收到硬體保護訊號，無條件進入 FAULT
            fsm->prev_state = fsm->current_state;
            fsm->current_state = PS_FAULT;
            if (fsm->ops->enable_pwm_output) {
                fsm->ops->enable_pwm_output(false);
            }
            // 讀取具體故障碼
            if (fsm->ops->get_hw_fault_status) {
                fsm->fault_code = fsm->ops->get_hw_fault_status();
            }
            break;

        case EVT_CLR_FAULT:
            if (fsm->current_state == PS_FAULT) {
                fsm->current_state = PS_IDLE;
                fsm->fault_code = 0;
            }
            break;

        default:
            break;
    }
}

// 週期性運作 (State Machine Body)
// 建議放在 1ms 或 10ms 的 Timer Task 中
void FSM_RunTick(PowerFSM *fsm) {
    if (fsm == NULL || fsm->ops == NULL) return;

    // 持續監控：如果在運行中發現硬體故障，自動觸發事件
    // (這是為了防止外部沒有呼叫 EVT_FAULT_TRIP)
    if (fsm->current_state != PS_FAULT && fsm->current_state != PS_IDLE) {
        if (fsm->ops->get_hw_fault_status() != 0) {
            FSM_HandleEvent(fsm, EVT_FAULT_TRIP);
            return;
        }
    }

    switch (fsm->current_state) {
        case PS_IDLE:
            // 什麼都不做，等待 EVT_CMD_ON
            fsm->soft_start_ratio = 0.0f;
            break;

        case PS_CHECK_INIT:
            // 檢查輸入電壓是否正常
            if (fsm->ops->check_input_voltage()) {
                // 通過檢查，進入軟啟動
                fsm->current_state = PS_SOFT_START;
                fsm->soft_start_ratio = 0.0f;
                fsm->ops->enable_pwm_output(true); // 開啟 PWM
            } else {
                // 檢查失敗，回到 IDLE (或者報錯)
                // 這裡簡單處理：卡在這裡或是退回 IDLE
                // fsm->current_state = PS_IDLE; 
            }
            break;

        case PS_SOFT_START:
            RunSoftStart(fsm);
            break;

        case PS_RUN:
            // 正常運作中
            // 可以在這裡加入 "Power Good" 訊號監控
            // 或是動態調整參數
            break;

        case PS_FAULT:
            // 故障鎖定中，等待 EVT_CLR_FAULT
            break;
    }
}

// 軟啟動邏輯 (內部 Helper)
static void RunSoftStart(PowerFSM *fsm) {
    // 每個 Tick 增加一點點參考電壓
    float step = 1.0f / (float)fsm->ss_duration_ticks;
    fsm->soft_start_ratio += step;

    if (fsm->soft_start_ratio >= 1.0f) {
        fsm->soft_start_ratio = 1.0f;
        fsm->current_state = PS_RUN; // 軟啟動完成，進入 RUN
    }

    // 更新硬體參考值
    fsm->ops->set_ref_voltage(fsm->soft_start_ratio);
}