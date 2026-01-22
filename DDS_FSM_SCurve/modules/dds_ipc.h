/**
 * @file dds_ipc.h
 * @brief CPU1 與 CPU2 的共享記憶體介面 (Thread-Safe Edition)
 */
#ifndef DDS_IPC_H
#define DDS_IPC_H
#include <stdint.h>
#include "dds_config.h"
#include "dds_defs.h" // Import Common Data Structure

// 定義資料結構 (IPC 內部使用，對外透明)
// [Refactor] 直接嵌入 DDS_Transfer_t 以確保欄位一致性 (Single Source of Truth)
typedef struct {
    DDS_Transfer_t data; // 核心資料
    
    volatile uint16_t seq_id;         
    volatile uint16_t padding; 
} DDS_IPC_Data_t;

extern volatile DDS_IPC_Data_t g_DDS_Shared;

// =========================================================================
// CPU1 寫入端 API (Commit)
// =========================================================================
// Input: DDS_Transfer_t (Pure Data)
static inline void DDS_IPC_Commit(DDS_IPC_Data_t* pDest, const DDS_Transfer_t* pSrc) {
    // Memcpy-like structure copy (Compilers optimize this well)
    pDest->data = *pSrc;

    __asm(" RPT #3 || NOP");
    pDest->seq_id++;
}

// =========================================================================
// CPU2 讀取端 API (Fetch)
// =========================================================================
// Output: DDS_Transfer_t (Pure Data)
static inline uint16_t DDS_IPC_Fetch(const DDS_IPC_Data_t* pSrc, DDS_Transfer_t* pLocal, uint16_t* pLastID) {
    uint16_t curr_id = pSrc->seq_id;
    if (curr_id != *pLastID) {
        *pLocal = pSrc->data; // Struct copy
        
        *pLastID = curr_id;
        return 1; 
    }
    return 0; 
}

#endif