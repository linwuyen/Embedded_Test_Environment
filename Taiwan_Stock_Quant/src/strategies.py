"""
交易策略模組
Trading Strategies Module
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Tuple
import sys
import os

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from config import (
    TREND_FOLLOWING_PARAMS, 
    MEAN_REVERSION_PARAMS, 
    HYBRID_STRATEGY_PARAMS,
    RISK_MANAGEMENT
)
from src.indicators import TechnicalIndicators


class BaseStrategy:
    """策略基礎類別"""
    
    def __init__(self, name: str):
        self.name = name
        self.signals = None
    
    def generate_signals(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        生成交易訊號
        
        Parameters:
        -----------
        df : pd.DataFrame
            包含技術指標的資料
            
        Returns:
        --------
        pd.DataFrame
            包含訊號的資料
        """
        raise NotImplementedError("子類別必須實作此方法")
    
    def get_signal_summary(self, signals: pd.DataFrame) -> Dict:
        """獲取訊號摘要統計"""
        buy_signals = (signals['signal'] == 1).sum()
        sell_signals = (signals['signal'] == -1).sum()
        hold_signals = (signals['signal'] == 0).sum()
        
        return {
            'strategy': self.name,
            'buy_signals': buy_signals,
            'sell_signals': sell_signals,
            'hold_signals': hold_signals,
            'total_signals': len(signals)
        }


class TrendFollowingStrategy(BaseStrategy):
    """趨勢跟隨策略"""
    
    def __init__(self):
        super().__init__("Trend Following Strategy")
        self.params = TREND_FOLLOWING_PARAMS
    
    def generate_signals(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        生成趨勢跟隨訊號
        
        策略邏輯:
        1. 黃金交叉 (短期均線上穿長期均線) + MACD > 0 + 成交量放大 → 買入
        2. 死亡交叉 (短期均線下穿長期均線) + MACD < 0 → 賣出
        
        Parameters:
        -----------
        df : pd.DataFrame
            包含技術指標的資料
            
        Returns:
        --------
        pd.DataFrame
            包含訊號的資料
        """
        df = df.copy()
        
        # 初始化訊號
        df['signal'] = 0
        df['position'] = 0
        
        # 均線交叉訊號
        df['ma_cross'] = 0
        df.loc[df['ma_short'] > df['ma_long'], 'ma_cross'] = 1
        df.loc[df['ma_short'] < df['ma_long'], 'ma_cross'] = -1
        
        # 黃金交叉與死亡交叉
        df['golden_cross'] = (
            (df['ma_cross'] == 1) & 
            (df['ma_cross'].shift(1) == -1)
        )
        df['death_cross'] = (
            (df['ma_cross'] == -1) & 
            (df['ma_cross'].shift(1) == 1)
        )
        
        # MACD 確認
        macd_bullish = df['macd'] > df['macd_signal']
        macd_bearish = df['macd'] < df['macd_signal']
        
        # 成交量確認
        volume_threshold = self.params['volume_threshold']
        volume_confirm = df['volume'] > (df['volume_ma'] * volume_threshold)
        
        # 買入訊號: 黃金交叉 + MACD 多頭 + 成交量放大
        buy_signal = (
            df['golden_cross'] & 
            macd_bullish & 
            volume_confirm
        )
        
        # 賣出訊號: 死亡交叉 + MACD 空頭
        sell_signal = (
            df['death_cross'] & 
            macd_bearish
        )
        
        df.loc[buy_signal, 'signal'] = 1
        df.loc[sell_signal, 'signal'] = -1
        
        # 計算持倉狀態
        df['position'] = df['signal'].replace(0, np.nan).ffill().fillna(0)
        
        return df


class MeanReversionStrategy(BaseStrategy):
    """均值回歸策略"""
    
    def __init__(self):
        super().__init__("Mean Reversion Strategy")
        self.params = MEAN_REVERSION_PARAMS
    
    def generate_signals(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        生成均值回歸訊號
        
        策略邏輯:
        1. RSI < 30 (超賣) + 價格觸及布林通道下軌 → 買入
        2. RSI > 70 (超買) + 價格觸及布林通道上軌 → 賣出
        
        Parameters:
        -----------
        df : pd.DataFrame
            包含技術指標的資料
            
        Returns:
        --------
        pd.DataFrame
            包含訊號的資料
        """
        df = df.copy()
        
        # 初始化訊號
        df['signal'] = 0
        df['position'] = 0
        
        # RSI 超買超賣
        rsi_oversold = df['rsi'] < self.params['rsi_oversold']
        rsi_overbought = df['rsi'] > self.params['rsi_overbought']
        
        # 布林通道位置
        # bb_position 接近 0 表示接近下軌，接近 1 表示接近上軌
        bb_lower_touch = df['bb_position'] < 0.2
        bb_upper_touch = df['bb_position'] > 0.8
        
        # 價格偏離均線程度
        price_deviation = abs(df['close'] - df['bb_middle']) / df['bb_middle']
        significant_deviation = price_deviation > self.params['price_deviation']
        
        # 買入訊號: RSI 超賣 + 觸及布林下軌 + 顯著偏離
        buy_signal = (
            rsi_oversold & 
            bb_lower_touch & 
            significant_deviation
        )
        
        # 賣出訊號: RSI 超買 + 觸及布林上軌
        sell_signal = (
            rsi_overbought & 
            bb_upper_touch
        )
        
        # 回歸訊號: 價格回到中軌附近
        mean_revert = (df['bb_position'] > 0.4) & (df['bb_position'] < 0.6)
        
        df.loc[buy_signal, 'signal'] = 1
        df.loc[sell_signal, 'signal'] = -1
        df.loc[mean_revert & (df['position'].shift(1) != 0), 'signal'] = 0
        
        # 計算持倉狀態
        df['position'] = df['signal'].replace(0, np.nan).ffill().fillna(0)
        
        return df


class HybridStrategy(BaseStrategy):
    """混合策略 (結合趨勢跟隨與均值回歸)"""
    
    def __init__(self):
        super().__init__("Hybrid Strategy")
        self.trend_strategy = TrendFollowingStrategy()
        self.mean_reversion_strategy = MeanReversionStrategy()
        self.params = HYBRID_STRATEGY_PARAMS
    
    def generate_signals(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        生成混合策略訊號
        
        策略邏輯:
        1. 結合趨勢跟隨與均值回歸訊號
        2. 使用加權平均決定最終訊號
        3. 需要多重確認才進場
        
        Parameters:
        -----------
        df : pd.DataFrame
            包含技術指標的資料
            
        Returns:
        --------
        pd.DataFrame
            包含訊號的資料
        """
        df = df.copy()
        
        # 獲取兩種策略的訊號
        df_trend = self.trend_strategy.generate_signals(df)
        df_mean = self.mean_reversion_strategy.generate_signals(df)
        
        # 初始化最終訊號
        df['signal'] = 0
        df['position'] = 0
        df['trend_signal'] = df_trend['signal']
        df['mean_signal'] = df_mean['signal']
        
        # 計算加權訊號
        trend_weight = self.params['trend_weight']
        mean_weight = self.params['mean_reversion_weight']
        
        df['weighted_signal'] = (
            df['trend_signal'] * trend_weight + 
            df['mean_signal'] * mean_weight
        )
        
        if self.params['confirmation_required']:
            # 需要多重確認: 兩種策略都同意才進場
            buy_signal = (df['trend_signal'] == 1) & (df['mean_signal'] == 1)
            sell_signal = (df['trend_signal'] == -1) & (df['mean_signal'] == -1)
            
            # 或者一個強烈訊號 + 另一個不反對
            buy_signal |= (
                (df['trend_signal'] == 1) & (df['mean_signal'] >= 0) & 
                (df['rsi'] < 50)  # RSI 確認
            )
            sell_signal |= (
                (df['trend_signal'] == -1) & (df['mean_signal'] <= 0) & 
                (df['rsi'] > 50)  # RSI 確認
            )
        else:
            # 不需要確認: 使用加權訊號
            buy_signal = df['weighted_signal'] > 0.5
            sell_signal = df['weighted_signal'] < -0.5
        
        df.loc[buy_signal, 'signal'] = 1
        df.loc[sell_signal, 'signal'] = -1
        
        # 最小持有天數限制
        min_holding_days = self.params['min_holding_days']
        if min_holding_days > 0:
            df = self._apply_min_holding_period(df, min_holding_days)
        
        # 計算持倉狀態
        df['position'] = df['signal'].replace(0, np.nan).ffill().fillna(0)
        
        return df
    
    def _apply_min_holding_period(
        self, 
        df: pd.DataFrame, 
        min_days: int
    ) -> pd.DataFrame:
        """應用最小持有期限制"""
        df = df.copy()
        position_changes = df['signal'] != 0
        
        for idx in df[position_changes].index:
            # 找到下一個訊號的位置
            next_signals = df.loc[idx:, 'signal'] != 0
            if next_signals.sum() > 1:
                next_idx = df.loc[idx:][next_signals].index[1]
                days_held = (next_idx - idx).days
                
                # 如果持有天數不足，取消下一個訊號
                if days_held < min_days:
                    df.loc[next_idx, 'signal'] = 0
        
        return df


class StrategyManager:
    """策略管理器"""
    
    def __init__(self):
        self.strategies = {
            'trend': TrendFollowingStrategy(),
            'mean_reversion': MeanReversionStrategy(),
            'hybrid': HybridStrategy()
        }
    
    def run_strategy(
        self, 
        strategy_name: str, 
        df: pd.DataFrame
    ) -> pd.DataFrame:
        """
        執行指定策略
        
        Parameters:
        -----------
        strategy_name : str
            策略名稱: 'trend', 'mean_reversion', 'hybrid'
        df : pd.DataFrame
            包含技術指標的資料
            
        Returns:
        --------
        pd.DataFrame
            包含訊號的資料
        """
        if strategy_name not in self.strategies:
            raise ValueError(f"未知策略: {strategy_name}")
        
        strategy = self.strategies[strategy_name]
        return strategy.generate_signals(df)
    
    def compare_strategies(
        self, 
        df: pd.DataFrame
    ) -> pd.DataFrame:
        """
        比較所有策略的訊號
        
        Parameters:
        -----------
        df : pd.DataFrame
            包含技術指標的資料
            
        Returns:
        --------
        pd.DataFrame
            策略比較摘要
        """
        summaries = []
        
        for name, strategy in self.strategies.items():
            signals = strategy.generate_signals(df)
            summary = strategy.get_signal_summary(signals)
            summaries.append(summary)
        
        return pd.DataFrame(summaries)


def test_strategies():
    """測試交易策略"""
    print("=" * 60)
    print("測試交易策略功能")
    print("=" * 60)
    
    # 建立測試資料
    dates = pd.date_range('2023-01-01', periods=200, freq='D')
    np.random.seed(42)
    
    # 模擬股價資料（帶趨勢）
    trend = np.linspace(100, 120, 200)
    noise = np.random.randn(200) * 3
    close = trend + noise
    
    high = close + np.random.rand(200) * 2
    low = close - np.random.rand(200) * 2
    volume = np.random.randint(1000000, 10000000, 200)
    
    df = pd.DataFrame({
        'close': close,
        'high': high,
        'low': low,
        'open': close,
        'volume': volume
    }, index=dates)
    
    # 添加技術指標
    df = TechnicalIndicators.add_all_indicators(df)
    
    # 測試策略管理器
    manager = StrategyManager()
    
    print("\n策略比較:")
    comparison = manager.compare_strategies(df)
    print(comparison.to_string(index=False))
    
    # 測試混合策略
    print("\n\n混合策略訊號範例:")
    hybrid_signals = manager.run_strategy('hybrid', df)
    signal_dates = hybrid_signals[hybrid_signals['signal'] != 0][
        ['close', 'signal', 'trend_signal', 'mean_signal']
    ]
    print(signal_dates.head(10))
    
    print("\n✅ 測試完成！")


if __name__ == "__main__":
    test_strategies()
