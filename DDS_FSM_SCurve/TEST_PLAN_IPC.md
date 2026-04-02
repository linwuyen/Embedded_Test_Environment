# IPC 與資料傳遞專項測試計畫 (IPC Test Plan)

本計畫專注於驗證 CPU1 與 CPU2 之間透過 `DDS_Transfer_t` 結構進行資料交換的 **完整性 (Integrity)**、**即時性 (Latency)** 與 **強健性 (Robustness)**。

---

## 1. 原子性破壞測試 (Atomicity Stress Test)
**目標**：證明在極端狀況下，CPU2 絕對不會讀到「寫入一半」的資料 (Data Tearing)。

### 測試場景
讓 CPU1 快速且連續地修改 `tuning_word` 的 **高 16-bit** 與 **低 16-bit**，觀察 CPU2 是否會讀到錯誤的組合。

### 測試程式碼 (Snippet)
*   **CPU1 (Service)**:
    ```c
    // 在 background loop 極速修改
    // Pattern A: 0x0000FFFF (低位全1)
    // Pattern B: 0xFFFF0000 (高位全1)
    static uint32_t pat = 0;
    pat = (pat == 0x0000FFFF) ? 0xFFFF0000 : 0x0000FFFF;
    dds.tuning_word = pat;
    DDS_Service_SyncIPC(&g_DDS_Shared);
    ```
*   **CPU2 (Core)**:
    ```c
    // 在 ISR 中檢查
    if (dds_local.tuning_word != 0x0000FFFF && dds_local.tuning_word != 0xFFFF0000) {
        // 如果讀到 0x00000000 或 0xFFFFFFFF，代表發生 Tearing (高低位不同步)
        ERROR_COUNT++; 
    }
    ```

### 通過標準 (Pass Criteria)
*   連續執行 1 小時，`ERROR_COUNT` 必須為 **0**。

---

## 2. 通訊延遲測試 (Latency Measurement)
**目標**：測量從「CPU1 決定改變頻率」到「CPU2 實際輸出新波形」的時間差。

### 測試方法 (使用 GPIO 示波器測量)
1.  **CPU1**: 在呼叫 `DDS_IPC_Commit` 之前，拉高 **GPIO-A**。
2.  **CPU2**: 在 `DDS_IPC_Fetch` 成功取得新資料後，拉高 **GPIO-B**。
3.  使用示波器測量 GPIO-A 上升緣 到 GPIO-B 上升緣 的時間差 $\Delta t$。

### 預期結果
*   **理論值**: $0 \sim T_{ISR}$ (因為 CPU2 是輪詢 IPC)。若是 100kHz ISR，延遲應在 **0us ~ 10us** 之間分佈。
*   **異常值**: 若 $> 20us$，代表 CPU2 的 ISR loading 過重，或 IPC 讀取效率太差。

---

## 3. 頻寬與吞吐量測試 (Throughput & Overrun)
**目標**：驗證當 CPU1 更新速度快於 CPU2 讀取速度時的行為 (Latest-Wins 策略)。

### 測試場景
*   **條件**: CPU1 設定為每 1us 更新一次 IPC (遠快於 CPU2 的 10us ISR)。
*   **觀察**: CPU2 的 `last_seq_id` 跳號情況。

### 預期結果
*   這不是錯誤，而是設計特性。
*   CPU2 應該只會處理「最新」的一筆資料。
*   **驗證點**: 系統不會因為 buffer overflow 而當機，也不會讀到舊的累積資料 (Queueing)。這證明了系統是 **Real-time (Latest-Wins)** 而非 **Buffered**。

---

## 4. 容錯性測試 (Fault Tolerance)
**目標**：驗證單邊失效對另一邊的影響。

| 測試項目 | 操作 | 預期結果 |
| :--- | :--- | :--- |
| **CPU2 當機** | 在 Debugger 中暫停 (Halt) CPU2 | CPU1 應繼續正常運作，Modbus 仍可通訊，不會因為 Shared RAM 沒人讀而卡死 (Non-blocking Write)。 |
| **CPU1 重置** | 手動 Reset CPU1 | CPU2 應偵測到 Watchdog 或能夠維持最後一個已知的有效輸出 (Failsafe)，直到 CPU1 恢復並送來新的 `seq_id`。 |

---

## 5. 記憶體邊界測試 (Memory Boundary)
**目標**：確保資料結構沒有越界存取。

### 檢查點
1.  查看 `.map` 檔案，確認 `g_DDS_Shared` 位於 `SHARERAMGS0` 的起始位址。
2.  確認 `sizeof(DDS_IPC_Data_t)` 大小是否符合預期，且有正確的 Memory Padding。
3.  **測試**: 在 Shared RAM 的結構體**前後**各放置一個 `Magic Pattern` (如 0xDEADBEEF)，運作一段時間後檢查 Pattern 是否被覆蓋。

---

## 6. 功能旗標正確性測試 (Feature Flag Propagation)
**目標**：驗證新增的功能旗標是否正確傳遞，無 Bit Shift 或 Mask 錯誤。

### 測試項目
1.  **Deadtime Enable**: CPU1 設定 `reg_Deadtime > 0`，檢查 CPU2 收到的 `feature_flags & DDS_FLAG_DEADTIME_EN` 是否為真。
2.  **Phase Cut Enable**: CPU1 設定 `reg_PhaseCutEn = 1`，檢查 CPU2 收到的 `feature_flags & DDS_FLAG_PHASE_CUT_EN`。
3.  **Slew Rate Bypass**: CPU1 設定 `reg_SlewRateEn = 0`，檢查 CPU2 收到的 flag 變化。

### 通過標準
*   旗標變化必須與 Modbus 設定完全同步。
*   檢查 `dds_defs.h` 中的 Flag Bit 定義是否與 `dds_core.c` 解碼邏輯一致。
