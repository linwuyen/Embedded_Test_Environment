"""
台股半導體量化交易系統 - 設定檔
Configuration file for Taiwan Semiconductor Quantitative Trading System
"""

from datetime import datetime

# ==================== 股票清單 ====================
# 台股半導體與 AI 相關概念股 (股票代碼需加 .TW 後綴用於 yfinance)
SEMI_AI_STOCKS = {
    # 晶圓代工 & 封測
    '2330.TW': '台積電',   # Wafer
    '2303.TW': '聯電',     # Wafer
    '3711.TW': '日月光投控', # Packaging
    '2449.TW': '京元電子', # Testing
    
    # IP & ASIC (AI 關鍵)
    '3443.TW': '創意',     # IP/ASIC
    '3661.TW': '世芯-KY',  # IP/ASIC
    '3035.TW': '智原',     # IP/ASIC
    
    # IC 設計
    '2454.TW': '聯發科',   # Mobile/Edge AI
    '2379.TW': '瑞昱',     # Comm/Edge
    '3034.TW': '聯詠',     # Driver IC
    '4966.TW': '譜瑞-KY',  # High speed interface
    '5274.TW': '信驊',     # BMC (Server)
    
    # AI 伺服器 & 組裝
    '2317.TW': '鴻海',     # Server/Assembly
    '2382.TW': '廣達',     # AI Server
    '3231.TW': '緯創',     # AI Server base
    '2376.TW': '技嘉',     # AI Server
    '2356.TW': '英業達',   # Server
    '6669.TW': '緯穎',     # Cloud Server
    '2357.TW': '華碩',     # AI PC/Server
    '2377.TW': '微星',     # AI PC/Server

    # 散熱 & 電源 & PCB
    '2308.TW': '台達電',   # Power/Thermal
    '3017.TW': '奇鋐',     # Thermal
    '3324.TW': '雙鴻',     # Thermal
    '3037.TW': '欣興',     # IC Substrate
    '2368.TW': '金像電',   # PCB
}

# 舊清單 (保留參考)
SEMICONDUCTOR_STOCKS = SEMI_AI_STOCKS

# ==================== 回測參數 ====================
BACKTEST_CONFIG = {
    'initial_capital': 10_000_000,  # 初始資金 (新台幣)
    'start_date': '2020-01-01',    # 回測起始日期
    'end_date': '2024-12-31',      # 回測結束日期
    'position_size': 0.2,          # 單筆最大投入比例 (20%)
    'max_positions': 3,            # 最大同時持股數量
}

# ==================== 交易成本 ====================
TRANSACTION_COSTS = {
    'commission_rate': 0.001425 * 0.6,  # 手續費率 (0.1425% 打六折)
    'tax_rate': 0.003,                   # 證券交易稅 (0.3%)
    'slippage': 0.001,                   # 滑價 (0.1%)
}

# ==================== 策略參數 ====================

# 趨勢跟隨策略參數
TREND_FOLLOWING_PARAMS = {
    'short_ma_period': 5,       # 短期均線週期 (原 20)
    'long_ma_period': 20,       # 長期均線週期 (原 60)
    'ma_type': 'EMA',           # 均線類型: 'SMA' or 'EMA'
    'macd_fast': 12,            # MACD 快線
    'macd_slow': 26,            # MACD 慢線
    'macd_signal': 9,           # MACD 訊號線
    'volume_ma_period': 20,     # 成交量均線週期
    'volume_threshold': 1.0,    # 成交量放大倍數 (原 1.2)
}

# 均值回歸策略參數
MEAN_REVERSION_PARAMS = {
    'rsi_period': 14,           # RSI 週期
    'rsi_oversold': 30,         # RSI 超賣閾值
    'rsi_overbought': 70,       # RSI 超買閾值
    'bb_period': 20,            # 布林通道週期
    'bb_std': 2.0,              # 布林通道標準差倍數
    'price_deviation': 0.05,    # 價格偏離度 (5%)
}

# 混合策略參數
HYBRID_STRATEGY_PARAMS = {
    'trend_weight': 0.5,        # 趨勢策略權重 (原 0.6)
    'mean_reversion_weight': 0.5,  # 均值回歸策略權重 (原 0.4)
    'confirmation_required': False,  # 是否需要多重確認
    'min_holding_days': 3,      # 最小持有天數
}

# ==================== 風險管理 ====================
RISK_MANAGEMENT = {
    'stop_loss': 0.10,          # 停損點 (10%) - 原 8%
    'take_profit': 0.50,        # 停利點 (50%) - 原 15%
    'trailing_stop': 0.15,      # 移動停損 (15%) - 原 5%
    'max_drawdown_limit': 0.20, # 最大回撤限制 (20%)
}

# ==================== 技術指標參數 ====================
INDICATOR_PARAMS = {
    'atr_period': 14,           # ATR 週期
    'adx_period': 14,           # ADX 週期
    'obv_ma_period': 20,        # OBV 均線週期
}

# ==================== 視覺化設定 ====================
VISUALIZATION_CONFIG = {
    'figure_size': (15, 10),
    'style': 'seaborn-v0_8-darkgrid',
    'save_format': 'png',
    'dpi': 300,
}

# ==================== 資料儲存路徑 ====================
DATA_PATHS = {
    'raw_data': 'data/raw/',
    'processed_data': 'data/processed/',
    'results': 'results/',
}

# ==================== 其他設定 ====================
MISC_CONFIG = {
    'random_seed': 42,
    'verbose': True,
    'save_results': True,
}
