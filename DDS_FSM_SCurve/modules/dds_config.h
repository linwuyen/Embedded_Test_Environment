/**
 * @file dds_config.h
 * @brief DDS 系統參數配置檔
 * @details 集中管理所有系統參數、記憶體區段定義與功能開關
 */
#ifndef DDS_CONFIG_H
#define DDS_CONFIG_H

// =========================================================================
// 1. 系統時脈與硬體設定
// =========================================================================
#define DDS_SYSCLK_FREQ_HZ      200000000.0f    // F28377D 主頻 200MHz
#define DDS_ISR_FREQ_HZ         100000.0f       // 中斷執行頻率 100kHz
#define DDS_PWM_TBPRD_VAL       (uint16_t)((DDS_SYSCLK_FREQ_HZ / DDS_ISR_FREQ_HZ) / 2) // PWM 用上下數模式

// =========================================================================
// 2. 記憶體區段 (Memory Sections)
// =========================================================================
// 用於 IPC 的共享記憶體區段名稱 (需配合 Linker CMD 檔)
#define DDS_SECTION_IPC_DATA    "SHARERAMGS0" 
#define DDS_SECTION_LUT_SINE    "ramgs0"
#define DDS_SECTION_LUT_ARB     "ramgs1"

// =========================================================================
// 3. 功能開關 (Feature Flags)
// =========================================================================
// [預留] 閉迴路控制 (PID/2p2z) - 目前僅作為 Stubs
#define DDS_ENABLE_CLOSE_LOOP_STUB  1 

// [預留] 硬體保護 (CMPSS/TZ) - 目前僅作為 Stubs
#define DDS_ENABLE_HARDWARE_PROTECT 1

// [預留] 任意波形 IPC 下載功能
#define DDS_ENABLE_ARB_WAVE_IPC     1

// =========================================================================
// 4. 控制參數預設值
// =========================================================================
#define DDS_DEFAULT_FREQ        60.0f
#define DDS_DEFAULT_VSET        0.0f    // 預設電壓為 0 (安全考量)
#define DDS_MAX_FREQ            2000.0f
#define DDS_MIN_FREQ            0.0f

#endif // DDS_CONFIG_H
