# Taiwan Stock Quantitative Trading System

台股半導體量化交易系統 - 結合趨勢跟隨與均值回歸策略的波段交易回測系統

## 📋 專案簡介

本專案是一個針對台股半導體產業的量化交易回測系統,結合了趨勢跟隨與均值回歸兩種策略,適用於波段交易。系統提供完整的資料獲取、技術指標計算、交易訊號生成、回測引擎以及視覺化分析功能。

## ✨ 主要功能

- 📊 **自動資料獲取**: 使用 yfinance 下載台股歷史資料
- 📈 **技術指標計算**: MA, MACD, RSI, 布林通道, ATR, KD 等
- 🎯 **多種交易策略**:
  - 趨勢跟隨策略 (均線交叉 + MACD)
  - 均值回歸策略 (RSI + 布林通道)
  - 混合策略 (結合上述兩種)
- 💰 **完整回測引擎**: 包含交易成本、停損停利、部位管理
- 📉 **績效分析**: 夏普比率、最大回撤、勝率等指標
- 📊 **視覺化報告**: 資金曲線、交易訊號、技術指標圖表

## 🚀 快速開始

### 安裝套件

```bash
pip install -r requirements.txt
```

### 執行回測

```bash
python main.py
```

### 使用 Jupyter Notebook 分析

```bash
jupyter notebook notebooks/analysis.ipynb
```

## 📁 專案結構

```
Taiwan_Stock_Quant/
├── data/                    # 資料目錄
│   ├── raw/                # 原始資料
│   └── processed/          # 處理後資料
├── src/                    # 原始碼
│   ├── data_loader.py      # 資料獲取模組
│   ├── indicators.py       # 技術指標計算
│   ├── strategies.py       # 交易策略
│   ├── backtester.py       # 回測引擎
│   └── visualizer.py       # 視覺化模組
├── notebooks/              # Jupyter Notebooks
│   └── analysis.ipynb      # 分析與測試
├── results/                # 回測結果
├── config.py               # 設定檔
├── main.py                 # 主程式
└── requirements.txt        # 套件需求
```

## ⚙️ 設定說明

在 `config.py` 中可以調整以下參數:

- **股票清單**: 修改 `SEMICONDUCTOR_STOCKS` 字典
- **回測期間**: 調整 `BACKTEST_CONFIG` 中的日期
- **策略參數**: 修改 `TREND_FOLLOWING_PARAMS` 和 `MEAN_REVERSION_PARAMS`
- **風險管理**: 調整 `RISK_MANAGEMENT` 中的停損停利點

## 📊 策略說明

### 趨勢跟隨策略
- 黃金交叉 + MACD 多頭 + 成交量放大 → 買入
- 死亡交叉 + MACD 空頭 → 賣出

### 均值回歸策略
- RSI < 30 + 觸及布林下軌 → 買入
- RSI > 70 + 觸及布林上軌 → 賣出

### 混合策略
- 結合兩種策略訊號
- 多重確認機制
- 最小持有天數限制

## 📈 績效指標

系統會計算以下績效指標:

- 總報酬率 / 年化報酬率
- 夏普比率 (Sharpe Ratio)
- 最大回撤 (Maximum Drawdown)
- 交易次數 / 勝率
- 平均獲利 / 平均虧損
- 獲利因子 (Profit Factor)

## ⚠️ 免責聲明

本系統僅供學習與研究使用,不構成任何投資建議。實際交易請自行評估風險,作者不對任何投資損失負責。

## 📝 授權

MIT License

## 👤 作者

Zerofivem

---

**注意**: 台股資料使用 yfinance 套件獲取,股票代碼需加 `.TW` 後綴。
