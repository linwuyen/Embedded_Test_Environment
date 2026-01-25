# 重新生成查找表說明

## 問題
DDS 查找表太大 (4096 點 × 2 表 = 32KB),超過可用 Flash 空間。

## 解決方案
已將 `LUT_SIZE` 從 4096 改為 1024,需要重新生成 `lut.c`。

## 步驟

1. 刪除舊的 lut.c:
```bash
rm /Users/zerofivemlin/Github/Test-Environment/F280049/DDS_Module/lut.c
```

2. 使用 Python 生成新的 1024 點查找表 (請執行以下 Python 腳本並保存為 lut.c)

或者,**更簡單的方法**:直接在 linker script 中將查找表放到 RAM:

在 `28004x_generic_flash_lnk.cmd` 的 SECTIONS 中加入:
```
.const:lut : > RAMGS2, PAGE = 1
```

這樣查找表會在啟動時從 Flash 複製到 RAM。
