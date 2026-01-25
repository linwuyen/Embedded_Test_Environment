#include "ring_buffer.h"

// 初始化
bool RB_Init(RingBuffer *rb, rb_item_t *buffer, uint16_t size) {
    if (rb == NULL || buffer == NULL || size < 2) {
        return false; // 防呆：指標無效或大小太小
    }
    
    rb->buffer = buffer;
    rb->capacity = size;
    rb->head = 0;
    rb->tail = 0;
    
    return true;
}

// 重置索引
void RB_Flush(RingBuffer *rb) {
    // 為了安全，建議在此處暫時關閉中斷 (視 MCU 而定)，但在通用庫中我們先只重置指標
    rb->head = 0;
    rb->tail = 0;
}

// 寫入資料 (Push)
bool RB_Write(RingBuffer *rb, rb_item_t data) {
    // 計算下一個寫入位置
    uint16_t next_head = (rb->head + 1);
    
    // 處理環形回繞 (Wrap around)
    if (next_head >= rb->capacity) {
        next_head = 0;
    }

    // 檢查是否已滿：如果下一個寫入位置等於讀取位置，表示滿了
    // 注意：這種實作方式會浪費一個 Slot，但能區分「空」與「滿」且不需額外變數
    if (next_head == rb->tail) {
        return false; // Buffer Full
    }

    // 關鍵順序：先寫入數據，再更新索引
    // 這樣在中斷讀取的情況下較安全，避免讀到髒數據
    rb->buffer[rb->head] = data;
    rb->head = next_head;

    return true;
}

// 讀取資料 (Pop)
bool RB_Read(RingBuffer *rb, rb_item_t *data) {
    // 檢查是否為空
    if (rb->head == rb->tail) {
        return false; // Buffer Empty
    }

    // 讀取數據
    *data = rb->buffer[rb->tail];

    // 計算下一個讀取位置
    uint16_t next_tail = (rb->tail + 1);
    if (next_tail >= rb->capacity) {
        next_tail = 0;
    }

    // 更新索引
    rb->tail = next_tail;

    return true;
}

// 取得目前資料量
uint16_t RB_GetCount(const RingBuffer *rb) {
    int32_t count = rb->head - rb->tail;
    if (count < 0) {
        count += rb->capacity;
    }
    return (uint16_t)count;
}

// 取得剩餘空間
uint16_t RB_GetFree(const RingBuffer *rb) {
    // 總容量 - 已用容量 - 1 (保留位)
    // 為什麼減 1？因為 head == tail 代表空，所以必須保留一個位置不能寫，否則無法區分滿與空
    return (rb->capacity - 1) - RB_GetCount(rb);
}

// 判斷滿
bool RB_IsFull(const RingBuffer *rb) {
    uint16_t next_head = (rb->head + 1);
    if (next_head >= rb->capacity) {
        next_head = 0;
    }
    return (next_head == rb->tail);
}

// 判斷空
bool RB_IsEmpty(const RingBuffer *rb) {
    return (rb->head == rb->tail);
}#include "ring_buffer.h"

// 初始化
bool RB_Init(RingBuffer *rb, rb_item_t *buffer, uint16_t size) {
    if (rb == NULL || buffer == NULL || size < 2) {
        return false; // 防呆：指標無效或大小太小
    }
    
    rb->buffer = buffer;
    rb->capacity = size;
    rb->head = 0;
    rb->tail = 0;
    
    return true;
}

// 重置索引
void RB_Flush(RingBuffer *rb) {
    // 為了安全，建議在此處暫時關閉中斷 (視 MCU 而定)，但在通用庫中我們先只重置指標
    rb->head = 0;
    rb->tail = 0;
}

// 寫入資料 (Push)
bool RB_Write(RingBuffer *rb, rb_item_t data) {
    // 計算下一個寫入位置
    uint16_t next_head = (rb->head + 1);
    
    // 處理環形回繞 (Wrap around)
    if (next_head >= rb->capacity) {
        next_head = 0;
    }

    // 檢查是否已滿：如果下一個寫入位置等於讀取位置，表示滿了
    // 注意：這種實作方式會浪費一個 Slot，但能區分「空」與「滿」且不需額外變數
    if (next_head == rb->tail) {
        return false; // Buffer Full
    }

    // 關鍵順序：先寫入數據，再更新索引
    // 這樣在中斷讀取的情況下較安全，避免讀到髒數據
    rb->buffer[rb->head] = data;
    rb->head = next_head;

    return true;
}

// 讀取資料 (Pop)
bool RB_Read(RingBuffer *rb, rb_item_t *data) {
    // 檢查是否為空
    if (rb->head == rb->tail) {
        return false; // Buffer Empty
    }

    // 讀取數據
    *data = rb->buffer[rb->tail];

    // 計算下一個讀取位置
    uint16_t next_tail = (rb->tail + 1);
    if (next_tail >= rb->capacity) {
        next_tail = 0;
    }

    // 更新索引
    rb->tail = next_tail;

    return true;
}

// 取得目前資料量
uint16_t RB_GetCount(const RingBuffer *rb) {
    int32_t count = rb->head - rb->tail;
    if (count < 0) {
        count += rb->capacity;
    }
    return (uint16_t)count;
}

// 取得剩餘空間
uint16_t RB_GetFree(const RingBuffer *rb) {
    // 總容量 - 已用容量 - 1 (保留位)
    // 為什麼減 1？因為 head == tail 代表空，所以必須保留一個位置不能寫，否則無法區分滿與空
    return (rb->capacity - 1) - RB_GetCount(rb);
}

// 判斷滿
bool RB_IsFull(const RingBuffer *rb) {
    uint16_t next_head = (rb->head + 1);
    if (next_head >= rb->capacity) {
        next_head = 0;
    }
    return (next_head == rb->tail);
}

// 判斷空
bool RB_IsEmpty(const RingBuffer *rb) {
    return (rb->head == rb->tail);
}