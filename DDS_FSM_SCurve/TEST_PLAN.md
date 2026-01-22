# DDS 系統驗證與測試計畫 (Test Plan)

本測試計畫旨在驗證 DDS 系統與 Tektronix 等級儀器的對標性能，涵蓋從純軟體邏輯到實際硬體輸出的完整測試流程。

---

## 1. 單元測試 (Unit Test) - 驗證軟體邏輯
**目標**：在不接硬體負載的情況下，確認演算法與狀態機邏輯正確。
**工具**：CCS Debugger (Expressions Window, Graph Tool)

| 測試項目 | 測試步驟 | 預期結果 |
| :--- | :--- | :--- |
| **LUT 查表驗證** | 1. 執行 `DDS_Global_Init()` <br> 2. 下斷點觀察 `LUT_Sine` 陣列 | `LUT_Sine[0]=0.0`, `LUT_Sine[256]=1.0` (約值), 波形平滑連續 |
| **頻率控制字驗證** | 1. 設定 `reg_Freq = 100.0Hz` <br> 2. 觀察 `dds.tuning_word` | `tuning_word` 應接近 `4294967` (計算式: $100 \times 2^{32} / 100000$) |
| **Soft Start 驗證** | 1. 設定 `reg_Enable = 0 -> 1` <br> 2. 觀察 `dds.v_set` | `v_set` 從 0.0 改為 **S-Curve** 平滑爬升至 1.0 |
| **IPC 原子性驗證** | 1. 在 CPU1 連續快速改變 `tuning_word` (0x00 ~ 0xFF) <br> 2. CPU2 設斷點在 Fetch 後 | CPU2 讀到的 `tuning_word` 永遠是 CPU1 寫入的完整值，絕無中間亂碼 |

---

## 2. 整合測試 (Integration Test) - 驗證雙核心協作
**目標**：確認 CPU1 (Service) 與 CPU2 (Core) 通訊順暢且控制即時。
**工具**：CCS Dual Core Debug + GPIO Toggle

| 測試項目 | 測試步驟 | 預期結果 |
| :--- | :--- | :--- |
| **心跳燈同步** | CPU1 與 CPU2 各自翻轉一個 LED (1Hz) | 兩個 LED 都穩定閃爍，代表雙核心皆無當機 |
| **跨核心參數生效** | 1. CPU1 修改 Modbus `reg_Freq` 為 50Hz, 60Hz 切換 <br> 2. 觀察 CPU2 PWM Duty | PWM Duty 週期隨 CPU1 指令即時變化，無明顯延遲 (>10ms) |
| **S-Curve Ramping** | 修改 `reg_Freq` 從 60Hz -> 120Hz | 頻率非瞬間跳變，而是依據 `reg_FreqTransTime` 設定的時間平滑過渡 (可用 Graph Tool 觀察 `g_DDS_Core.freq_current`) |

---

## 3. 控制狀態機驗證 (Control FSM Verification)
**目標**：確保系統狀態流轉符合設計，無卡死或非法狀態。
**觀察點**: `reg_CurrentState` (Modbus Addr, Read Only)

| 測試項目 | 初始狀態 | 觸發動作 | 預期結果 |
| :--- | :--- | :--- | :--- |
| **手動啟動** | IDLE | Set `reg_Enable = 1` | 進入 **RUN_MANUAL**，輸出開始 Soft Start |
| **手動停止** | RUN_MANUAL | Set `reg_Enable = 0` | 回到 **IDLE**，Ramp Target/Current 強制歸零 |
| **序列執行** | RUN_MANUAL | Set `reg_SeqRun = 1` | 進入 **RUN_SEQUENCE**，開始執行 Step 1 |
| **序列中斷** | RUN_SEQUENCE | Set `reg_SeqRun = 0` | 回到 **RUN_MANUAL**，維持當前頻率電壓 |
| **故障觸發** | Any | (模擬) 設定 Fault Flags | 進入 **FAULT**，輸出強制為 0，鎖定操作 |
| **故障復歸** | FAULT | Set `reg_Enable = 0` | 回到 **IDLE** (必須先 Disable 才能清除 Fault) |

---

## 4. 進階功能驗證 (Advanced Features)

### 4.1 序列引擎 (Sequencer)
1.  **編程**: 使用 `DDS_Service_WriteSequence` 寫入:
    *   Step 0: 60Hz, 100V, 5s
    *   Step 1: 50Hz, 110V, 3s
    *   Step 2: 400Hz, 0V, 1s
2.  **執行**: 啟動 `reg_SeqRun = 1`。
3.  **驗證**: 觀察輸出頻率/電壓是否精確依照時間表變化，無延遲或跳步。

### 4.2 相位切割 (Phase Cut / Dimmer)
1.  **設定**: `reg_PhaseCutEn = 1`，設定 `PhaseCutOn = 90.0`, `PhaseCutOff = 270.0`。
2.  **驗證**: 示波器應顯示正弦波僅在 90~270 度區間有輸出，其餘為 0 (類似 Dimmer 效果)。

### 4.3 任意波形 (Arbitrary Waveform)
1.  **載入**: 在 IDLE 狀態下，透過 UART/Modbus 寫入 LUT_ARB 大量數據。
2.  **執行**: 設定 `reg_WaveType = DDS_WAVE_ARB`，啟動系統。
3.  **驗證**: 輸出波形應完全符合載入的點數據。

---

## 5. 硬體驗證 (Hardware Validation) - 驗證訊號品質
**目標**：接上示波器與負載，驗證最終輸出品質。
**工具**：示波器 (Oscilloscope)、頻譜分析儀 (Spectrum Analyzer)

| 測試項目 | 測試波形 / 條件 | 觀察重點 (Pass Criteria) |
| :--- | :--- | :--- |
| **頻率精準度** | 設定 60.00Hz, 400.00Hz |誤差應 < 0.01% (取決於晶振) |
| **波形失真度 (THD)** | 輸出正弦波 @ 100% Load | 波形圓滑，無明顯階梯 (由 LC 濾波決定)，Zero-Crossing 處無振盪 |
| **軟啟動行為** | 觸發設在 Enable 訊號上升緣 | 輸出電壓呈現完美的 **S-Curve** 爬升 (Ramp-up envelope)，無瞬間過衝 (Overshoot) |
| **Jitter / Dither 測試** | 開啟/關閉 `reg_DitherEn` | 開啟後，觀察頻譜分析儀 (FFT)，基頻峰值稍微降低，底噪變寬但變平滑 (EMI 優化) |

---

## 6. 保護機制測試 (Safety Test)
**目標**：**破壞性測試**，確保系統能保護自身。
**注意**：建議先使用限流電源供應器進行測試。

| 測試項目 | 測試步驟 | 預期結果 |
| :--- | :--- | :--- |
| **急停測試 (Soft Stop)** | 運作中將 Enable 拉低 | 電壓在設定時間內 (如 100ms) 緩降至 0V，非直接切斷 |
| **Trip Zone 觸發 (模擬)** | 手動觸發 PWM TZ 旗標 | PWM 訊號應在 **單一週期內 (microseconds)** 立即停止輸出 |
| **看門狗測試 (Watchdog)** | 故意在 CPU2 寫死迴圈 `while(1);` | 系統應在 Watchdog 時間到後自動重置 (Reset) |
