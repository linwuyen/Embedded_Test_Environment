# TI C2000 DDS 雙核心通訊手冊 (F2837xD)

本文件說明 DDS 專案中 CPU1 (主控端) 與 CPU2 (運算端) 之間的通訊協議與實作細節。本架構採用「解耦式設計 (Decoupled Design)」，確保高安全性與可移植性。

## 1. 架構總覽

| 角色 | 模組 | 職責 | 運作頻率 |
| :--- | :--- | :--- | :--- |
| **生產者 (Producer)** | `dds_service` | 計算 Ramping、執行序列、處理 Modbus 指令 | 背景迴圈 (Background) |
| **搬運工 (Transport)** | `dds_ipc` | 定義共享記憶體結構，負責搬運資料 | 依需求呼叫 |
| **消費者 (Consumer)** | `dds_core` | 執行 DDS 查表、相位累加、硬體保護 | 高速中斷 (100kHz ISR) |

### 核心設計理念
1.  **資料結構通用化**：使用 `DDS_Transfer_t` 作為通用語言，Service 與 Core 均不依賴 IPC。
2.  **存取原子性 (Atomicity)**：透過「寫入 -> 屏障 -> 旗標」機制，杜絕資料撕裂 (Data Tearing)。
3.  **單向控制流**：CPU1 僅負責寫入，CPU2 僅負責讀取，無 Race Condition 風險。

---

## 2. 資料結構 (`dds_defs.h` & `dds_ipc.h`)

### 2.1 通用傳輸物件 (`DDS_Transfer_t`)
這是各模組間交換資料的標準格式 (`dds_defs.h`)：
```c
typedef struct {
    uint32_t tuning_word;   // 頻率控制字
    float    v_set;         // 電壓設定 (0.0 ~ 1.0)
    float    v_offset;      // 直流偏壓
    uint16_t wave_type;     // 波形選擇
    uint16_t enable;        // 總開關
    
    // 進階功能控制
    uint16_t feature_flags; // Bit0: Dither, Bit1: 3rdHarmonic, Bit2: Deadtime, Bit3: PhaseCut
    float    harmonic_3rd_gain; // 3次諧波增益
    float    slew_rate;         // Slew Rate 限制 (V/sample)
    uint16_t deadtime_ticks;    // 死區時間 (Ticks)
    float    dither_amount;     // 抖動量
    float    phase_cut_on;      // 相位切割 (開啟角)
    float    phase_cut_off;     // 相位切割 (關閉角)
} DDS_Transfer_t;
```

### 2.2 IPC 共享容器 (`DDS_IPC_Data_t`)
這是放在 Shared RAM (`SHARERAMGS0`) 中的容器 (`dds_ipc.h`)：
```c
typedef struct {
    DDS_Transfer_t data;       // 直接嵌入傳輸物件 (Payload)
    
    // 同步用關鍵欄位
    volatile uint16_t seq_id;  // 序列編號 (版本號)
    volatile uint16_t padding; // 補齊 32-bit 對齊
} DDS_IPC_Data_t;
```

---

## 3. 通訊流程 (Handshake Protocol)

我們採用 **"Publish-Subscribe"** 模式，由 `seq_id` 作為版本控制。

### 步驟 A: CPU1 發布資料 (Commit)
位於 `main_cpu1.c` -> `DDS_IPC_Commit()`

1.  **準備資料**：`dds_service` 計算出最新的 `DDS_Transfer_t`。
2.  **寫入 Payload**：將資料寫入 Shared RAM。
3.  **記憶體屏障 (Memory Barrier)**：執行 `__asm(" RPT #3 || NOP");`，確保編譯器與 CPU Pipeline 已將資料完全寫入緩衝區。
4.  **發布版本號**：`seq_id++`。此動作必須是最後一步。

### 步驟 B: CPU2 接收資料 (Fetch)
位於 `main_cpu2.c` (ISR) -> `DDS_IPC_Fetch()`

1.  **檢查版本**：比較 `SharedRAM.seq_id` 與本地的 `last_seq_id`。
2.  **版本相同**：代表無新資料，直接返回，不進行記憶體 copy (節省 ISR 時間)。
3.  **版本不同**：
    *   從 Shared RAM 複製整包資料到本地 cache。
    *   更新 `last_seq_id`。
    *   呼叫 `DDS_ApplySettings()` 更新核心參數。

---

## 4. 移植指南 (Single Core Migration)

若需將此專案移植到單核心晶片 (如 F280049C)，只需修改 `main.c` 的連接方式，**無需修改任何模組程式碼**：

**雙核模式 (目前):**
```c
// CPU1
Service_GetData(&data);
IPC_Commit(&shared, &data);

// CPU2
IPC_Fetch(&shared, &data);
Core_Apply(&core, &data);
```

**單核模式 (未來):**
```c
// CPU1 Only
Service_GetData(&data);
// 直接傳遞，跳過 IPC
Core_Apply(&core, &data); 
```

---

## 5. 常見問題 (FAQ)

*   **Q: 為什麼不直接用指標讀取 Shared RAM?**
    *   A: 當 CPU1 正在寫入 32-bit `tuning_word` 的高 16-bit 時，若 CPU2 剛好讀取，會讀到「新高位 + 舊低位」的錯誤數值，導致頻率瞬間飄移。我們的 `seq_id` 機制解決了這個問題。
*   **Q: 支援多快更新率?**
    *   A: CPU2 在 100kHz 中斷內運行。只要 CPU1 更新 `seq_id`，CPU2 下一個中斷週期 (10us內) 就會生效。