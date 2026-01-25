#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// --- 設定區 ---
// 在 C2000 或其他 MCU，如果要存 16-bit 數據，可將此改為 uint16_t
typedef uint8_t rb_item_t; 

// --- 結構定義 ---
// 使用 struct 封裝，實現物件導向概念，方便同時宣告多個 Buffer (如 UART_RX, UART_TX)
typedef struct {
    rb_item_t *buffer;  // 指向實際儲存數據的陣列 (外部傳入，避免 malloc)
    uint16_t capacity;  // 緩衝區總大小
    volatile uint16_t head; // 寫入位置 (volatile 確保編譯器不優化讀取)
    volatile uint16_t tail; // 讀取位置
} RingBuffer;

// --- API 介面 ---

/**
 * @brief 初始化 Ring Buffer
 * @param rb 指向 RingBuffer 結構的指標
 * @param buffer 指向實際記憶體陣列的指標
 * @param size 陣列的大小
 * @return true 初始化成功, false 參數錯誤
 */
bool RB_Init(RingBuffer *rb, rb_item_t *buffer, uint16_t size);

/**
 * @brief 清空 Buffer (重置索引)
 */
void RB_Flush(RingBuffer *rb);

/**
 * @brief 寫入一筆資料 (非阻塞)
 * @param data 要寫入的數據
 * @return true 寫入成功, false 緩衝區已滿
 */
bool RB_Write(RingBuffer *rb, rb_item_t data);

/**
 * @brief 讀取一筆資料 (非阻塞)
 * @param data 指向接收數據的指標
 * @return true 讀取成功, false 緩衝區為空
 */
bool RB_Read(RingBuffer *rb, rb_item_t *data);

/**
 * @brief 檢查目前有多少資料可讀
 */
uint16_t RB_GetCount(const RingBuffer *rb);

/**
 * @brief 檢查目前還剩多少空間可寫
 */
uint16_t RB_GetFree(const RingBuffer *rb);

/**
 * @brief 檢查是否已滿
 */
bool RB_IsFull(const RingBuffer *rb);

/**
 * @brief 檢查是否為空
 */
bool RB_IsEmpty(const RingBuffer *rb);

#endif // RING_BUFFER_H