# DDS 與 SPWM 並存整合完成報告

## ✅ 整合完成!

成功將 DDS 模組整合到 F280049 專案,與 SPWM_Ctrl 並存,提供靈活的模式切換功能。

---

## 📁 新增模組結構

```
F280049/
├── DDS_Module/              [新增] DDS 波形產生器
│   ├── DDS.c               (9.8 KB)
│   ├── DDS.h               (4.6 KB)
│   ├── DDS_UserConfig.h    (1.6 KB)
│   ├── lut.c               (102 KB) 查找表
│   └── lut.h               (292 bytes)
│
├── WaveGen_Manager/         [新增] 模式管理器
│   ├── WaveGen_Manager.h
│   └── WaveGen_Manager.c
│
├── SPWM_Ctrl.c/h           [保留] 原有 SPWM
├── ADC_Module/             [保留]
├── CLA_Module/             [保留]
└── Diagnostic/             [保留]
```

---

## 🎯 核心功能

### 1. 雙模式支援

**SPWM 模式** (原有):
- 標準 PWM 波形產生
- 適合一般應用

**DDS 模式** (新增):
- 高精度頻率控制 (~0.000028 Hz)
- Q24 定點運算
- 完整狀態機 (4 狀態)
- Slew Rate 平滑控制
- Soft Start/Stop

### 2. 統一控制介面

```c
// 模式切換
WaveGen_SetMode(WAVEGEN_MODE_DDS);   // 切換到 DDS
WaveGen_SetMode(WAVEGEN_MODE_SPWM);  // 切換到 SPWM

// 參數設定 (兩種模式通用)
WaveGen_SetFrequency(60.0f);         // 設定頻率
WaveGen_SetAmplitude(1.65f);         // 設定振幅
WaveGen_SetOffset(1.65f);            // 設定偏移
WaveGen_SetPhase(0.0f);              // 設定相位
WaveGen_Enable(1);                   // 啟用輸出
```

### 3. 自動路由

系統會根據當前模式自動路由到對應的波形產生器:

```c
void WaveGen_ISR_Update(void)
{
    if (mode == DDS) {
        // 使用 DDS 產生波形
        uint16_t dacValue = DDS_Run_Core(&DDS);
        // 輸出到 DAC/PWM
    } else {
        // 使用 SPWM 產生波形
        SPWM_UpdateOutput();
    }
}
```

---

## 📊 整合成果

### 新增檔案
- **DDS 模組**: 5 個檔案 (~118 KB)
- **管理器**: 2 個檔案
- **總計**: 7 個新檔案

### 更新檔案
- `common.h` - 加入 DDS 和管理器
- `main.c` - 初始化 WaveGen_Manager
- `timetask.c` - 加入 WaveGen_Task

---

## 🔧 使用範例

### 範例 1: 使用 SPWM 模式

```c
// 初始化時設定為 SPWM 模式
WaveGen_SetMode(WAVEGEN_MODE_SPWM);
WaveGen_SetFrequency(60.0f);
WaveGen_SetAmplitude(1.65f);
WaveGen_Enable(1);
```

### 範例 2: 切換到 DDS 模式

```c
// 切換到 DDS 模式以獲得更高精度
WaveGen_SetMode(WAVEGEN_MODE_DDS);

// 設定 DDS 特有參數
DDS.Cmd.fSlewRate_sec = 2.0f;      // 2 秒平滑過渡
DDS.Cmd.u32DelayOn_ms = 100;       // 100ms 開啟延遲

// 使用統一介面設定參數
WaveGen_SetFrequency(50.0f);       // 50 Hz
WaveGen_Enable(1);

// 平滑切換到 60 Hz (2 秒過渡)
WaveGen_SetFrequency(60.0f);
```

### 範例 3: 透過 SPI 命令切換模式

```c
// 在 SPI 命令處理中加入模式切換
void SPI_HandleCommand(uint16_t cmd, uint16_t data)
{
    switch(cmd)
    {
    case CMD_SET_WAVEGEN_MODE:
        if (data == 0) {
            WaveGen_SetMode(WAVEGEN_MODE_SPWM);
        } else {
            WaveGen_SetMode(WAVEGEN_MODE_DDS);
        }
        break;
        
    case CMD_SET_FREQUENCY:
        WaveGen_SetFrequency((float)data);
        break;
    }
}
```

---

## 📈 模式比較

| 特性 | SPWM 模式 | DDS 模式 |
|------|-----------|----------|
| **頻率精度** | ~0.1 Hz | ~0.000028 Hz |
| **參數切換** | 直接跳變 | Slew Rate 平滑 |
| **狀態管理** | 簡單 | 4 狀態機 |
| **啟停控制** | 直接 | Soft Start/Stop |
| **CPU 負載** | 低 | 中 (Q24 運算) |
| **記憶體** | 小 | 大 (~120 KB) |
| **適用場景** | 一般應用 | 高精度應用 |

---

## ⚙️ 系統流程

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
  └─> WaveGen_Task()
        ├─> [DDS 模式] DDS_Task()
        └─> [SPWM 模式] SPWM_Task()
```

---

## 🎯 應用場景

### 使用 SPWM 模式
- ✅ 一般馬達控制
- ✅ 標準變頻器應用
- ✅ 不需要高精度的場合

### 使用 DDS 模式
- ✅ 精密測試設備
- ✅ 高精度頻率合成
- ✅ 需要平滑參數變化
- ✅ 需要完整狀態控制

### 模式切換場景
- ✅ 測試模式 vs 運行模式
- ✅ 高精度校準 vs 正常運行
- ✅ 不同應用場景切換

---

## ⚠️ 注意事項

### 1. 記憶體使用

DDS 模組需要約 120 KB 記憶體 (主要是查找表),確保 Flash 空間足夠。

### 2. 模式切換

切換模式時會自動停止當前輸出,建議在系統空閒時切換。

### 3. ISR 頻率

DDS 建議使用 120kHz ISR 以獲得最佳效果。

---

## 📝 後續步驟

### 必要工作

1. **編譯設定**
   - 在 CCS 中加入 DDS_Module 到 Include Paths
   - 加入 DDS.c, lut.c 到編譯列表

2. **測試驗證**
   - SPWM 模式功能測試
   - DDS 模式功能測試
   - 模式切換測試

### 選用工作

1. **SPI 整合**
   - 加入模式切換命令
   - 加入參數設定命令

2. **診斷功能**
   - 加入 DDS 測試模式
   - 加入模式狀態回報

3. **效能優化**
   - CLA 加速 DDS 查表
   - 記憶體優化

---

## 🎉 整合優勢

✅ **保留原有功能** - SPWM 模式完整保留  
✅ **新增高精度模式** - DDS 提供極高精度  
✅ **靈活切換** - 可根據需求選擇模式  
✅ **統一介面** - 簡化使用與維護  
✅ **向後相容** - 不影響現有程式碼  

---

**整合完成日期**: 2026-01-26  
**整合版本**: v2.0 (DDS + SPWM 並存)
