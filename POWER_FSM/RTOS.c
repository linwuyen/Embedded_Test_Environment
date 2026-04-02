/*
 * 這是一個範例，展示如何將我們寫的 "PowerFSM" 放入 RTOS (如 FreeRTOS)
 * * 架構說明：
 * 1. ISR (中斷): 負責極高速的 ADC/PWM 控制 (這是 C2000 的命脈，不經 RTOS)
 * 2. Task (任務): 負責 1ms ~ 10ms 等級的邏輯，如狀態機 (FSM)
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "power_fsm.h"      // 引用你的狀態機
#include "protocol_parser.h"// 引用你的解析器

// --- 全域物件 ---
PowerFSM g_PowerFSM;
ProtocolParser g_Parser;

// 定義一個佇列 (Queue) 用來從 ISR 傳送指令給 Task
// 這樣做可以避免 Race Condition
QueueHandle_t xCommandQueue;

// --- 1. 高速控制迴路 (ISR) - 這是 Bare Metal 部分 ---
// 頻率：例如 100kHz (每 10us 一次)
// 這裡絕對不能跑 RTOS API (除了 FromISR 結尾的函式)，越快結束越好
__interrupt void EPWM1_TZ_ISR(void) {
    // 1. 讀取 ADC
    // 2. 算 PID
    // 3. 更新 PWM
    
    // 4. (選用) 如果發生嚴重硬體故障，直接通知 Task
    if (HW_Check_OVP()) {
        PowerEvent_t evt = EVT_FAULT_TRIP;
        // 發送訊息給狀態機 Task (不會阻塞 ISR)
        xQueueSendFromISR(xCommandQueue, &evt, NULL);
    }
    
    // 清除中斷旗標
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

// --- 2. 狀態機任務 (RTOS Task) ---
// 頻率：由 vTaskDelay 控制，例如 10ms 跑一次
// 負責：開關機流程、溫度監控、與外部通訊
void vPowerFSMTask(void *pvParameters) {
    
    // 初始化 FSM (跟在 main 裡做的一樣)
    // const PowerOps hw_ops = { ... }; // 定義硬體操作
    // FSM_Init(&g_PowerFSM, &hw_ops);

    PowerEvent_t received_evt;

    for (;;) {
        // A. 處理來自 ISR 或 通訊 Task 的指令
        // portMAX_DELAY 代表如果沒指令就睡覺 (Block)，完全不耗 CPU
        // 這裡我們用 0，代表不等待，只檢查有沒有新指令
        if (xQueueReceive(xCommandQueue, &received_evt, 0) == pdTRUE) {
            FSM_HandleEvent(&g_PowerFSM, received_evt);
        }

        // B. 執行狀態機邏輯 (這就是我寫的那個 RunTick)
        FSM_RunTick(&g_PowerFSM);

        // C. 讓出 CPU 權限給別人，休息 10ms
        // 這就是 RTOS 的好處，時間到自動喚醒，不用自己數 Timer
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

// --- 3. 通訊任務 (RTOS Task) ---
// 負責：把收到的資料丟給 Parser
void vCommsTask(void *pvParameters) {
    uint8_t rx_byte;
    
    for (;;) {
        // 假設從 UART Queue 收到一個 Byte
        if (UART_Receive(&rx_byte)) {
            
            // 呼叫你的解析器
            Parser_InputByte(&g_Parser, rx_byte);
            
            // 注意：Callback 裡面應該發送 Event 給 vPowerFSMTask
            // 而不是直接操作 FSM，這樣才是 Thread-safe
        }
    }
}

// --- 4. Main ---
void main() {
    // 系統時脈初始化...
    
    // 建立 Queue
    xCommandQueue = xQueueCreate(10, sizeof(PowerEvent_t));

    // 建立 Tasks
    xTaskCreate(vPowerFSMTask, "PowerFSM", 256, NULL, 2, NULL); // 優先權 2
    xTaskCreate(vCommsTask,    "Comms",    256, NULL, 1, NULL); // 優先權 1

    // 啟動排程器 (開始多工)
    vTaskStartScheduler();
    
    // 程式不會跑來這
    for(;;);
}