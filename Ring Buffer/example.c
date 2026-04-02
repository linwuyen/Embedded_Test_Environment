#include <stdio.h>
#include "ring_buffer.h"

// --- 模擬 MCU 的記憶體配置 ---

// [應用場景 1] UART 通訊緩衝區
#define UART_BUF_SIZE 64
rb_item_t uart_mem[UART_BUF_SIZE]; // 實體記憶體
RingBuffer uart_rb;                // 管理結構 (控制代碼)

// [應用場景 2] ADC 採樣緩衝區 (用於做滑動平均)
#define ADC_BUF_SIZE 16
rb_item_t adc_mem[ADC_BUF_SIZE];   // 實體記憶體
RingBuffer adc_rb;                 // 管理結構 (控制代碼)


// --- 模擬中斷 (Producers) ---

// 1. UART RX 中斷：接收 PC 指令
void UART_RX_ISR(uint8_t cmd_byte) {
    // 把收到的 byte 丟進 UART 專用的 buffer
    if (!RB_Write(&uart_rb, cmd_byte)) {
        // 處理 UART Buffer 滿溢錯誤
        printf("[ISR] UART Buffer Overflow!\n");
    }
}

// 2. ADC 轉換完成中斷：讀取電壓值
// 假設這是高頻率觸發 (例如 50kHz)
void ADC_EOC_ISR(uint8_t voltage_val) {
    // 把 ADC 數值丟進 ADC 專用的 buffer
    // 注意：實際 C2000 ADC 通常是 uint16_t，這裡為了示範沿用 uint8_t
    // 若要存 16-bit，只需去 ring_buffer.h 修改 typedef 即可
    RB_Write(&adc_rb, voltage_val);
    
    // ADC 採樣通常允許舊資料被覆蓋(視策略而定)，
    // 或是滿了就不存，這裡我們用標準的滿了就不存。
}


// --- 模擬主迴圈 (Consumers) ---

void Main_Process() {
    rb_item_t data;

    // [任務 A] 處理 UART 指令
    // 只要 UART Buffer 裡有東西，就一直讀出來處理
    while (RB_Read(&uart_rb, &data)) {
        printf("[Main] Processing CMD: 0x%02X\n", data);
        if (data == 0xFF) {
            printf("       -> Reset Command Detected!\n");
        }
    }

    // [任務 B] 計算 ADC 平均值
    // 假設我們每收集 10 筆資料就算一次平均
    if (RB_GetCount(&adc_rb) >= 10) {
        uint32_t sum = 0;
        uint16_t samples = 0;
        
        // 連續讀出 10 筆
        for (int i = 0; i < 10; i++) {
            if (RB_Read(&adc_rb, &data)) {
                sum += data;
                samples++;
            }
        }
        
        if (samples > 0) {
            printf("[Main] ADC Average (10 samples): %d\n", sum / samples);
        }
    }
}

int main() {
    // --- 系統初始化 ---
    
    // 1. 初始化 UART Buffer
    if (!RB_Init(&uart_rb, uart_mem, UART_BUF_SIZE)) {
        printf("UART Buffer Init Failed!\n");
        return -1;
    }

    // 2. 初始化 ADC Buffer
    if (!RB_Init(&adc_rb, adc_mem, ADC_BUF_SIZE)) {
        printf("ADC Buffer Init Failed!\n");
        return -1;
    }

    printf("System Initialized.\n");
    printf("-------------------\n");

    // --- 模擬運行 ---

    // 1. 模擬 UART 收到一些指令
    printf("Simulating UART Interrupts...\n");
    UART_RX_ISR(0x01); // 開機指令
    UART_RX_ISR(0x02); // 調整電壓指令
    UART_RX_ISR(0xFF); // 重置指令

    // 2. 模擬 ADC 高速採樣 (連續進來 12 筆數據)
    printf("Simulating ADC Interrupts...\n");
    for (int i = 0; i < 12; i++) {
        // 模擬電壓在 100 ~ 112 之間波動
        ADC_EOC_ISR(100 + i); 
    }

    // 3. 主迴圈處理
    printf("Entering Main Loop...\n");
    Main_Process();

    // 4. 再次檢查剩餘資料
    printf("-------------------\n");
    printf("UART Remaining: %d\n", RB_GetCount(&uart_rb));
    printf("ADC Remaining: %d (Should be 2, because we processed 10)\n", RB_GetCount(&adc_rb));

    return 0;
}