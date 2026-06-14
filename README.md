# Embedded_Test_Environment

Embedded firmware 測試環境與驗證流程 repository。

此 repo 用於整理可重複執行的測試流程、測試輸入、測試輸出格式與驗證紀錄。它不是正式韌體主線。

## 1. Repository 角色

- **用途：** embedded firmware test environment。
- **狀態：** test / tooling repository。
- **可見性：** public。
- **範圍：** 測試案例、測試流程、報告格式、工具設定說明。
- **正式產品狀態：** 不是 production firmware。

## 2. 適合放的內容

- 測試案例說明
- 驗證流程
- input / output 範例
- report template
- tool setup notes
- target board 測試條件

## 3. Test case template

```text
Test name:
Purpose:
Target firmware / board:
Input files:
Command:
Expected output:
Pass criteria:
Known limitations:
```

## 4. Working rules

1. 每個 test workflow 都要能重現。
2. 測試前提、輸入、輸出與 pass criteria 要寫清楚。
3. 測試結果與測試定義要分開管理。
4. 大型輸出結果不要直接堆進 repo。
