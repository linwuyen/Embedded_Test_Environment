# DDS 專案移植手冊 (Migration Guide)

本手冊旨在指導開發者將此 DDS 專案移植到其他硬體平台（如 TI C2000 單核心系列 F28004x、F28002x，或他牌 MCU）。

由於本專案採用 **邏輯與傳輸解耦 (Decoupled Architecture)** 設計，移植過程將非常直觀。新版本引入了 **狀態機 (FSM)** 與 **S-Curve Ramping**，這些邏輯完全封裝於 `dds_service.c` 層，移植時可直接沿用。

---

## 1. 移植到 TI 單核心晶片 (如 F280049C)

單核心晶片沒有 CPU2，因此我們需要將 `Service` (慢速邏輯) 與 `Core` (高速運算) 合併到同一個 `main.c` 中。

### 步驟 1.1：移除 IPC 層
單核心不需要 IPC 通訊。
*   **刪除檔案**：`modules/dds_ipc.h`
*   **專案設定**：移除 Linker Command File 中對 `SHARERAMGS0` 的依賴。
*   **資料結構**：`dds_defs.h` 定義了跨模組通用的 `DDS_Transfer_t`，此檔案必須保留。

### 步驟 1.2：合併 Main Loop
建立一個新的 `main.c`，將原本分散在兩顆 CPU 的工作合併：

```c
#include "modules/dds_service.h"
#include "modules/dds_core.h"
#include "modules/dds_config.h"

// 定義全域變數
DDS_Handle_t g_DDS_Core;         // 核心物件
DDS_Transfer_t g_Data_Transfer;  // 資料交換緩衝區

// 背景迴圈 (原本的 CPU1)
void main(void) {
    Device_init();
    
    // 初始化模組
    DDS_Service_Init();
    DDS_Global_Init();
    DDS_InitObj(&g_DDS_Core, DDS_ISR_FREQ_HZ);
    
    // 設定 PWM 與 1ms Timer 中斷...
    
    while(1) {
        // 1. 執行應用層邏輯 (FSM, Ramping)
        DDS_Service_Run_Background();
        
        // 2. [關鍵] 直接資料傳遞 (Bypass IPC)
        // 從 Service 取出最新狀態
        DDS_Service_GetData(&g_Data_Transfer);
        
        // 直接套用到 Core (需注意執行緒安全，建議暫時關閉 ISR)
        DINT; 
        DDS_ApplySettings(&g_DDS_Core, &g_Data_Transfer);
        EINT;
    }
}

// 1ms 計時器中斷 (用於驅動 Ramping)
__interrupt void timer_isr(void) {
    DDS_Service_Tick(); // 提供 Service Layer 1ms 時基
}

// 高速 EPWM 中斷 (原本的 CPU2)
__interrupt void epwm_isr(void) {
    // 執行核心運算
    float out = DDS_Update(&g_DDS_Core);
    
    //寫入 PWM 暫存器...
    // EPWM_write...
}
```

---

## 2. 修改硬體配置 (`dds_config.h`)

不同晶片的時脈與 PWM 解析度不同，請修改 `modules/dds_config.h`：

### 2.1 系統時脈
*   **F28377D**: 200 MHz
*   **F280049**: 100 MHz
*   **F280025**: 100 MHz

```c
// 修改此行
#define DDS_SYSCLK_FREQ_HZ      100000000.0f  // 針對 F280049 降頻
```

### 2.2 記憶體區段
若新晶片沒有 `ramgs0` (Global Shared RAM)，請修改 LUT 的存放位置：

```c
// 修改 modules/dds_config.h
// 單核晶片通常放在 "ramls0" (Local Shared RAM)
#define DDS_SECTION_LUT_SINE    "ramls0"
#define DDS_SECTION_LUT_ARB     "ramls1"
```

---

## 3. 硬體保護介面實作

請務必針對新晶片的保護機制（Trip Zone / CMPSS）實作 `dds_core.c` 中的 Stubs：

```c
static inline void DDS_Hardware_Protect_Check(DDS_Handle_t* pDDS) {
#if DDS_ENABLE_HARDWARE_PROTECT
    // 範例：讀取 F28004x 的 TZFLG
    if (EPWM_getTripZoneFlagStatus(EPWM1_BASE) & EPWM_TZ_FLAG_OST) {
        pDDS->v_set = 0.0f; // 強制歸零
    }
#endif
}
```

---

## 4. 新功能支援 (FSM & S-Curve)

本專案引入了以下高階控制邏輯，在移植時需注意：

1.  **S-Curve Ramping**: 此功能完全由 `dds_service.c` 計算，僅依賴 `DDS_Service_Tick()` (1ms) 來確保計時準確。移植時請務必配置一個 1kHz Timer 來呼叫 Tick。
2.  **Finite State Machine (FSM)**: 狀態機 (IDLE, MANUAL, SEQUENCE, FAULT) 位於 Service 層。
3.  **Phase Cut (Dimmer)**: 由 Core 層執行。請確保新晶片的 PWM 模組支援高解析度 (HRPWM) 以獲得最佳相位控制效果，否則可能會有些微抖動。
