# Modbus 暫存器位址表 (Modbus Register Map)

本文件定義上位機 (PC/HMI) 與 DDS 系統通訊的暫存器位址。
**Base Address**: 0 (或 40001, 視 Master 定義而定)

## 1. 讀寫參數 (Read/Write Parameters)

| Address (Hex) | Address (Dec) | Name | Type | Unit | Range | Description |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **0x0000** | 0 | `reg_Freq` | Float32 | Hz | 0.0 - 2000.0 | 輸出頻率 (跨 2 Words) |
| **0x0002** | 2 | `reg_Vset` | Float32 | Ratio | 0.0 - 1.0 | 輸出電壓調變率 (0.0=0V, 1.0=Max) |
| **0x0004** | 4 | `reg_Voffset` | Float32 | V | +/- 10.0 | 直流偏壓 (未完全實作 HW) |
| **0x0006** | 6 | `reg_Deadtime` | Uint16 | Ticks | 0 - 500 | 死區時間 (0=Disable) |
| **0x0007** | 7 | `reg_WaveType` | Uint16 | Enum | 0-2 | 0=Sine, 1=Square, 2=Triangle, 3=Arb |
| **0x0008** | 8 | `reg_Enable` | Uint16 | Bool | 0/1 | 總開關 (0=Disable, 1=Enable) |
| **0x0009** | 9 | `reg_3rdHarmonic` | Float32 | Ratio | 0.0 - 0.5 | 3次諧波注入增益 (跨 2 Words) |
| **0x000B** | 11 | `reg_DitherEn` | Uint16 | Bool | 0/1 | 頻譜抖動開關 |
| **0x000C** | 12 | `reg_FreqTransTime` | Float32 | Sec | 0.1 - 10.0 | 頻率爬升時間 (S-Curve) |
| **0x000E** | 14 | `reg_VoltTransTime` | Float32 | Sec | 0.1 - 10.0 | 電壓建立時間 (S-Curve) |
| **0x0010** | 16 | `reg_PhaseCutOn` | Float32 | Deg | 0.0 - 360.0 | 相位切割開啟角 (Dimmer) |
| **0x0012** | 18 | `reg_PhaseCutOff` | Float32 | Deg | 0.0 - 360.0 | 相位切割關閉角 |
| **0x0014** | 20 | `reg_PhaseCutEn` | Uint16 | Bool | 0/1 | 相位切割總開關 |
| **0x0015** | 21 | `reg_DitherAmt` | Float32 | Ratio | 0.0 - 0.1 | 抖動強度 (預設 0.03) |
| **0x0017** | 23 | `reg_SlewRateEn` | Uint16 | Bool | 0/1 | 電壓爬升率限制 (1=Enable Limit) |
| **0x0018** | 24 | `reg_RampEn` | Uint16 | Bool | 0/1 | **Master Ramp Switch** (0=Instant) |
| **0x0019** | 25 | `reg_SeqRun` | Uint16 | Bool | 0/1 | 序列執行開關 (1=Run Sequence) |

## 2. 唯讀狀態 (Read-Only Status)

| Address (Hex) | Name | Type | Value | Description |
| :--- | :--- | :--- | :--- | :--- |
| **0x001A** | `reg_CurrentState` | Uint16 | Enum | 0=Idle, 1=Manual, 2=Sequence, 3=Fault |
| **0x001B** | `reg_FaultCode` | Uint16 | Enum | 0=Normal, 1=OVP, 2=OCP |

## 3. 資料型態說明
*   **Float32**: IEEE-754 單精度浮點數，佔用 2 個 Registers (Little Endian words, High word first assumption depending on Modbus Lib).
    *   *Note: 因為 `Modbus_Float_t` 定義為 Union，需確認實際傳輸時的高低位順序。*
*   **Uint16**: 無號整數，佔用 1 個 Register。

## 4. 特殊功能操作
### 4.1 任意波形下載 (Arbitrary Waveform)
無法透過標準暫存器直接寫入。請使用特殊 Function Code 或連續寫入特定 Buffer (需實作 Modbus Extension)。
*   目前僅支援透過 API `DDS_Service_WriteArbWave` 在韌體內部寫入。

### 4.2 序列編程 (Sequencer Programming)
目前僅支援透過 API `DDS_Service_WriteSequence` 在韌體內部寫入。若需透過 Modbus 修改序列，建議擴充位址 **0x0100** 開始的區段對映到 `g_Sequence` 陣列。
