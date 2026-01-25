# F280049 完整控制系統專案

高度整合的 TI C2000 F280049 控制系統,整合 DDS、SPWM、ADC、CLA 等多個功能模組。

## 📋 專案簡介

本專案是一個功能完整的嵌入式控制系統,適用於 TI F280049 微控制器,整合了多個高級功能模組,提供靈活的波形產生、閉迴路控制、運算加速等功能。

### 核心特色

- 🌊 **雙模式波形產生**: SPWM + DDS 可切換
- 📊 **閉迴路控制**: ADC 電流/電壓回授
- ⚡ **CLA 加速**: 協處理器運算加速
- 🔧 **系統診斷**: 完整的測試與診斷功能
- 📡 **SPI 通訊**: 主從機通訊支援
- 🛡️ **保護機制**: 過流/過壓/欠壓保護

---

## 🏗️ 專案架構

```
F280049/
├── DDS_Module/              DDS 高精度波形產生器
│   ├── DDS.c               核心實作 (Q24 定點運算)
│   ├── DDS.h               介面定義
│   ├── DDS_UserConfig.h    使用者配置
│   ├── lut.c               查找表 (102 KB)
│   └── lut.h               查找表定義
│
├── WaveGen_Manager/         波形產生器管理
│   ├── WaveGen_Manager.h   模式管理介面
│   └── WaveGen_Manager.c   SPWM/DDS 切換邏輯
│
├── ADC_Module/              ADC 採樣模組
│   ├── adc_config.h        ADC 配置參數
│   ├── adc_control.h       ADC 控制介面
│   └── adc_control.c       採樣、濾波、保護
│
├── CLA_Module/              CLA 運算加速
│   ├── cla_shared.h        共享記憶體定義
│   ├── cla_tasks.h         任務介面
│   ├── cla_tasks.cla       CLA 程式 (SPWM/PID/Clarke/Park)
│   └── cla_tasks.c         CPU 控制函數
│
├── Diagnostic/              診斷測試模組
│   ├── diag_crystal.h      診斷介面
│   └── diag_crystal.c      晶振/PWM/ADC/CLA 測試
│
├── SPWM_Ctrl.c/h           SPWM 控制 (原有)
├── spi_master.h            SPI 主機
├── spi_slave.h             SPI 從機
├── timetask.c/h            時間任務系統
├── main.c                  主程式
└── common.h                共用標頭檔
```

---

## ✨ 功能模組

### 1. DDS 模組 (高精度波形產生)

**功能**:
- Q24 定點運算 (頻率精度 ~0.000028 Hz)
- 4 狀態機管理 (STOPPED → WAIT_ON → RUNNING → WAIT_OFF)
- Slew Rate 平滑控制
- Soft Start/Stop
- 多波形支援 (正弦、梯形、DC)

**使用範例**:
```c
// 切換到 DDS 模式
WaveGen_SetMode(WAVEGEN_MODE_DDS);

// 設定參數
WaveGen_SetFrequency(60.0f);    // 60 Hz
WaveGen_SetAmplitude(1.65f);    // 1.65V
DDS.Cmd.fSlewRate_sec = 2.0f;   // 2 秒平滑過渡
WaveGen_Enable(1);
```

### 2. SPWM 模組 (標準 PWM 控制)

**功能**:
- 三相 PWM 波形產生
- 標準 SPWM 調變
- 適合一般馬達控制

**使用範例**:
```c
// 切換到 SPWM 模式
WaveGen_SetMode(WAVEGEN_MODE_SPWM);
WaveGen_SetFrequency(60.0f);
WaveGen_Enable(1);
```

### 3. ADC 模組 (電流/電壓採樣)

**功能**:
- 4 通道採樣 (電流 A/B/C + DC 電壓)
- 自動單位轉換
- 低通濾波
- 過流/過壓/欠壓保護
- 10kHz 背景處理

**配置**:
```c
#define OVERCURRENT_THRESHOLD   8.0f    // A
#define OVERVOLTAGE_THRESHOLD   350.0f  // V
#define ADC_FILTER_ALPHA        0.1f    // 濾波係數
```

**存取數據**:
```c
float current = g_ADC_Data.f32Current_A_Filt;
float voltage = g_ADC_Data.f32Voltage_DC_Filt;
if (g_ADC_Data.u16OverCurrent) {
    // 觸發保護
}
```

### 4. CLA 模組 (運算加速)

**功能**:
- Task 1: SPWM 三相占空比計算 (3.3x 加速)
- Task 2: PID 控制器 (2.5x 加速)
- Task 3: Clarke 轉換 (abc → αβ)
- Task 4: Park 轉換 (αβ → dq)

**使用範例**:
```c
// 觸發 SPWM 計算
CLA_TriggerSPWM(phase_angle, modulation_index);

// 使用結果
float dutyA = Cla1ToCpu.f32DutyA;
```

### 5. 診斷模組

**功能**:
- 晶振頻率測試
- PWM 波形測試
- ADC 功能測試
- CLA 功能測試

**使用範例**:
```c
Diag_SetMode(DIAG_MODE_CRYSTAL_TEST);
// 用示波器測量 GPIO16 頻率
Diag_ExitTest();
```

### 6. WaveGen_Manager (模式管理)

**功能**:
- SPWM/DDS 模式切換
- 統一控制介面
- ISR 自動路由

**API**:
```c
WaveGen_SetMode(mode);          // 切換模式
WaveGen_SetFrequency(freq_hz);  // 設定頻率
WaveGen_SetAmplitude(amp_v);    // 設定振幅
WaveGen_Enable(enable);         // 啟用/停用
```

---

## 🚀 快速開始

### 硬體需求

- TI F280049 微控制器
- 12-bit DAC 或 PWM 輸出
- ADC 輸入 (電流/電壓感測器)
- ePWM 模組
- SPI 介面 (選用)

### 軟體需求

- Code Composer Studio (CCS)
- C2000Ware SDK
- SysConfig 工具

### 編譯步驟

1. **開啟專案**
   - 在 CCS 中匯入專案

2. **設定 Include Paths**
   ```
   ${PROJECT_ROOT}/DDS_Module
   ${PROJECT_ROOT}/ADC_Module
   ${PROJECT_ROOT}/CLA_Module
   ${PROJECT_ROOT}/WaveGen_Manager
   ${PROJECT_ROOT}/Diagnostic
   ```

3. **加入原始檔**
   - DDS_Module/DDS.c
   - DDS_Module/lut.c
   - ADC_Module/adc_control.c
   - CLA_Module/cla_tasks.c
   - CLA_Module/cla_tasks.cla (CLA Compiler)
   - WaveGen_Manager/WaveGen_Manager.c
   - Diagnostic/diag_crystal.c

4. **編譯並燒錄**

---

## 📊 系統流程

### 初始化流程

```
main()
  ├─> Device_init()
  ├─> Board_init()
  ├─> ADC_Module_Init()
  ├─> CLA_Module_Init()
  ├─> Diag_Init()
  └─> WaveGen_Init()
        ├─> DDS_Init()
        └─> SPWM_Init()
```

### 運行時流程

```
EPWM ISR (120kHz)
  └─> WaveGen_ISR_Update()
        ├─> [DDS 模式] DDS_Run_Core()
        └─> [SPWM 模式] SPWM_Update()

Time Task (1kHz)
  ├─> task_ADC_Process()    (100us)
  ├─> WaveGen_Task()        (1ms)
  └─> task1msec()           (1ms)
```

---

## 🎯 使用範例

### 範例 1: 基本 DDS 波形產生

```c
void example_basic_dds(void)
{
    // 切換到 DDS 模式
    WaveGen_SetMode(WAVEGEN_MODE_DDS);
    
    // 設定參數
    WaveGen_SetFrequency(60.0f);     // 60 Hz
    WaveGen_SetAmplitude(1.65f);     // 1.65V
    WaveGen_SetOffset(1.65f);        // 1.65V 偏移
    
    // 設定 DDS 特有參數
    DDS.Cmd.fSlewRate_sec = 1.0f;    // 1 秒平滑
    DDS.Cmd.u32DelayOn_ms = 100;     // 100ms 延遲
    
    // 啟用輸出
    WaveGen_Enable(1);
}
```

### 範例 2: 平滑頻率變化

```c
void example_smooth_frequency_change(void)
{
    // DDS 模式支援平滑變化
    WaveGen_SetMode(WAVEGEN_MODE_DDS);
    DDS.Cmd.fSlewRate_sec = 3.0f;    // 3 秒過渡
    
    // 從 50Hz 平滑過渡到 60Hz
    WaveGen_SetFrequency(50.0f);
    WaveGen_Enable(1);
    
    // 等待 5 秒
    DEVICE_DELAY_US(5000000);
    
    // 平滑切換到 60Hz (3 秒過渡)
    WaveGen_SetFrequency(60.0f);
}
```

### 範例 3: ADC 監控與保護

```c
void example_adc_monitoring(void)
{
    // ADC 自動在背景處理
    
    // 讀取濾波後的數據
    float current_a = g_ADC_Data.f32Current_A_Filt;
    float voltage = g_ADC_Data.f32Voltage_DC_Filt;
    
    // 檢查保護狀態
    if (g_ADC_Data.u16OverCurrent) {
        // 過流保護觸發
        WaveGen_Enable(0);  // 停止輸出
    }
}
```

### 範例 4: CLA 加速運算

```c
void example_cla_acceleration(void)
{
    // 觸發 SPWM 計算
    float phase = 0.0f;
    float modulation = 0.8f;
    CLA_TriggerSPWM(phase, modulation);
    
    // 等待 CLA 完成
    while(!CLA_getTaskRunStatus(CLA1_BASE, CLA_TASKFLAG_1));
    
    // 使用計算結果
    float dutyA = Cla1ToCpu.f32DutyA;
    float dutyB = Cla1ToCpu.f32DutyB;
    float dutyC = Cla1ToCpu.f32DutyC;
}
```

---

## ⚙️ 配置參數

### DDS 配置 (DDS_UserConfig.h)

```c
#define DAC_FULL_SCALE      4095        // DAC 最大值
#define V_BASE              3.3f        // 電壓基準
#define ISR_FREQ_HZ         120000.0    // ISR 頻率
#define DDS_MAX_FREQ_HZ     1000.0f     // 最大頻率
```

### ADC 配置 (adc_config.h)

```c
#define OVERCURRENT_THRESHOLD   8.0f    // 過流閾值 (A)
#define OVERVOLTAGE_THRESHOLD   350.0f  // 過壓閾值 (V)
#define ADC_FILTER_ALPHA        0.1f    // 濾波係數
```

---

## 📈 效能指標

| 項目 | 數值 |
|------|------|
| **DDS 頻率精度** | ~0.000028 Hz |
| **ADC 採樣率** | 10 kHz |
| **SPWM 更新率** | 120 kHz |
| **CPU 負載** | ~35% |
| **記憶體使用** | ~150 KB Flash, 2 KB RAM |

---

## 🔧 故障排除

### 問題 1: 編譯錯誤 - 找不到標頭檔

**解決方案**: 檢查 Include Paths 設定,確保所有模組目錄都已加入。

### 問題 2: DDS 輸出無波形

**檢查項目**:
- `WaveGen_Enable(1)` 是否已呼叫
- 檢查 `g_WaveGen_Status.eMode` 是否為 `WAVEGEN_MODE_DDS`
- 確認 EPWM ISR 正常觸發

### 問題 3: ADC 數據異常

**檢查項目**:
- 確認 ADC 通道配置正確
- 檢查感測器連接
- 驗證單位轉換參數

---

## 📚 相關文件

- [DDS_SPWM_整合完成報告.md](./DDS_SPWM_整合完成報告.md) - 整合詳情
- [整合完成報告.md](./整合完成報告.md) - ADC/CLA/Diagnostic 整合
- [專案完整度比較.md](../專案完整度比較.md) - 與其他專案比較

---

## 🎯 應用場景

- ✅ 變頻器控制
- ✅ 馬達驅動
- ✅ 精密測試設備
- ✅ 電源轉換器
- ✅ 信號產生器

---

## 📄 授權

本專案僅供學習與研究使用。

---

## 👤 維護者

Zerofivem

**最後更新**: 2026-01-26
