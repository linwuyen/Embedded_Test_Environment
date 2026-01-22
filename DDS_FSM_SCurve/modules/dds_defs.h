/**
 * @file dds_defs.h
 * @brief DDS 通用資料結構定義
 * @details 定義各模組間交換的純資料結構，不含任何函式邏輯，確保模組解耦
 */
#ifndef DDS_DEFS_H
#define DDS_DEFS_H
#include <stdint.h>

/**
 * @brief 控制參數傳輸結構 (Data Transfer Object)
 * @details 用於 Service -> Core 之間的參數傳遞
 *          在雙核模式下，此結構會被序列化到 IPC Shared Memory
 *          在單核模式下，此結構直接由參數傳遞
 */
typedef struct {
    uint32_t tuning_word;   // 頻率控制字
    float    v_set;         // 電壓設定
    float    v_offset;      // 電壓偏移
    uint16_t wave_type;     // 波形種類
    uint16_t enable;        // 總致能開關

    // [New] 進階功能參數與開關
    uint16_t feature_flags; // Bit 0: Dither, Bit 1: 3rd Harmonic, Bit 2: SlewRate Limit
    float    harmonic_3rd_gain;
    float    dither_amount; // [New] 0.0 - 0.1
    float    slew_rate;     // V/sample
    uint16_t deadtime_ticks;
    uint16_t phase_cut_on;  // [New] Start Angle (0-360)
    uint16_t phase_cut_off; // [New] End Angle (0-360)
} DDS_Transfer_t;

// Feature Flag Definitions
#define DDS_FLAG_DITHER_EN      (1 << 0)
#define DDS_FLAG_3RD_HARM_EN    (1 << 1)
#define DDS_FLAG_SLEW_LIMIT_EN  (1 << 2)
#define DDS_FLAG_DEADTIME_EN    (1 << 3)
#define DDS_FLAG_PHASE_CUT_EN   (1 << 4) // [New]



#endif // DDS_DEFS_H
