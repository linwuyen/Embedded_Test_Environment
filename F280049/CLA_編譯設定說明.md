# CLA 模組編譯設定說明

## 問題

CLA 模組的符號 (`CpuToCla1`, `Cla1ToCpu`, `g_CLA_PID`) 在連結時未定義,因為 `cla_tasks.cla` 檔案需要使用 CLA 編譯器編譯。

## 解決方案

### 在 CCS 中加入 CLA 檔案到編譯

1. **右鍵點擊專案** → Properties
2. **Build** → **C2000 Compiler** → **Include Options**
   - 確認已加入 `CLA_Module` 到 include paths

3. **右鍵點擊 `CLA_Module/cla_tasks.cla`** → **File Properties**
4. **在 Build 頁籤中**:
   - 確認 "Exclude from build" **未勾選**
   - Tool: 選擇 **CLA Compiler**

5. **Build** → **CLA Compiler** → **Include Options**
   - 加入相同的 include paths

### 啟用 CLA 初始化

在 `main.c` 中,取消註解:

```c
CLA_Module_Init();      // Initialize CLA module
```

### 驗證

編譯後檢查:
- `Cla1Prog` 區段應該出現在 map 檔案中
- CLA 符號應該被解析

## 暫時方案

目前 CLA 初始化已被註解掉,專案可以編譯但 CLA 功能無法使用。

要使用 CLA 加速功能,請按照上述步驟設定 CLA 編譯器。
