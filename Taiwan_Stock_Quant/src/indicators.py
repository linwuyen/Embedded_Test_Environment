"""
技術指標計算模組
Technical Indicators Module
"""

import pandas as pd
import numpy as np
from typing import Tuple, Optional
import sys
import os

# 加入父目錄到路徑
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from config import TREND_FOLLOWING_PARAMS, MEAN_REVERSION_PARAMS, INDICATOR_PARAMS


class TechnicalIndicators:
    """技術指標計算類別"""
    
    @staticmethod
    def calculate_sma(data: pd.Series, period: int) -> pd.Series:
        """
        計算簡單移動平均線 (Simple Moving Average)
        
        Parameters:
        -----------
        data : pd.Series
            價格序列
        period : int
            週期
            
        Returns:
        --------
        pd.Series
            SMA 值
        """
        return data.rolling(window=period).mean()
    
    @staticmethod
    def calculate_ema(data: pd.Series, period: int) -> pd.Series:
        """
        計算指數移動平均線 (Exponential Moving Average)
        
        Parameters:
        -----------
        data : pd.Series
            價格序列
        period : int
            週期
            
        Returns:
        --------
        pd.Series
            EMA 值
        """
        return data.ewm(span=period, adjust=False).mean()
    
    @staticmethod
    def calculate_ma(
        data: pd.Series, 
        period: int, 
        ma_type: str = 'SMA'
    ) -> pd.Series:
        """
        計算移動平均線
        
        Parameters:
        -----------
        data : pd.Series
            價格序列
        period : int
            週期
        ma_type : str
            均線類型: 'SMA' 或 'EMA'
            
        Returns:
        --------
        pd.Series
            MA 值
        """
        if ma_type.upper() == 'EMA':
            return TechnicalIndicators.calculate_ema(data, period)
        else:
            return TechnicalIndicators.calculate_sma(data, period)
    
    @staticmethod
    def calculate_macd(
        data: pd.Series,
        fast_period: int = 12,
        slow_period: int = 26,
        signal_period: int = 9
    ) -> Tuple[pd.Series, pd.Series, pd.Series]:
        """
        計算 MACD 指標
        
        Parameters:
        -----------
        data : pd.Series
            價格序列
        fast_period : int
            快線週期
        slow_period : int
            慢線週期
        signal_period : int
            訊號線週期
            
        Returns:
        --------
        Tuple[pd.Series, pd.Series, pd.Series]
            (MACD, Signal, Histogram)
        """
        ema_fast = TechnicalIndicators.calculate_ema(data, fast_period)
        ema_slow = TechnicalIndicators.calculate_ema(data, slow_period)
        
        macd = ema_fast - ema_slow
        signal = TechnicalIndicators.calculate_ema(macd, signal_period)
        histogram = macd - signal
        
        return macd, signal, histogram
    
    @staticmethod
    def calculate_rsi(data: pd.Series, period: int = 14) -> pd.Series:
        """
        計算相對強弱指標 (Relative Strength Index)
        
        Parameters:
        -----------
        data : pd.Series
            價格序列
        period : int
            週期
            
        Returns:
        --------
        pd.Series
            RSI 值 (0-100)
        """
        delta = data.diff()
        gain = (delta.where(delta > 0, 0)).rolling(window=period).mean()
        loss = (-delta.where(delta < 0, 0)).rolling(window=period).mean()
        
        rs = gain / loss
        rsi = 100 - (100 / (1 + rs))
        
        return rsi
    
    @staticmethod
    def calculate_bollinger_bands(
        data: pd.Series,
        period: int = 20,
        std_dev: float = 2.0
    ) -> Tuple[pd.Series, pd.Series, pd.Series]:
        """
        計算布林通道 (Bollinger Bands)
        
        Parameters:
        -----------
        data : pd.Series
            價格序列
        period : int
            週期
        std_dev : float
            標準差倍數
            
        Returns:
        --------
        Tuple[pd.Series, pd.Series, pd.Series]
            (Upper Band, Middle Band, Lower Band)
        """
        middle_band = TechnicalIndicators.calculate_sma(data, period)
        std = data.rolling(window=period).std()
        
        upper_band = middle_band + (std * std_dev)
        lower_band = middle_band - (std * std_dev)
        
        return upper_band, middle_band, lower_band
    
    @staticmethod
    def calculate_atr(
        high: pd.Series,
        low: pd.Series,
        close: pd.Series,
        period: int = 14
    ) -> pd.Series:
        """
        計算平均真實波幅 (Average True Range)
        
        Parameters:
        -----------
        high : pd.Series
            最高價序列
        low : pd.Series
            最低價序列
        close : pd.Series
            收盤價序列
        period : int
            週期
            
        Returns:
        --------
        pd.Series
            ATR 值
        """
        tr1 = high - low
        tr2 = abs(high - close.shift())
        tr3 = abs(low - close.shift())
        
        tr = pd.concat([tr1, tr2, tr3], axis=1).max(axis=1)
        atr = tr.rolling(window=period).mean()
        
        return atr
    
    @staticmethod
    def calculate_volume_ma(volume: pd.Series, period: int = 20) -> pd.Series:
        """
        計算成交量移動平均
        
        Parameters:
        -----------
        volume : pd.Series
            成交量序列
        period : int
            週期
            
        Returns:
        --------
        pd.Series
            成交量均線
        """
        return volume.rolling(window=period).mean()
    
    @staticmethod
    def calculate_obv(close: pd.Series, volume: pd.Series) -> pd.Series:
        """
        計算能量潮指標 (On-Balance Volume)
        
        Parameters:
        -----------
        close : pd.Series
            收盤價序列
        volume : pd.Series
            成交量序列
            
        Returns:
        --------
        pd.Series
            OBV 值
        """
        obv = (np.sign(close.diff()) * volume).fillna(0).cumsum()
        return obv
    
    @staticmethod
    def calculate_stochastic(
        high: pd.Series,
        low: pd.Series,
        close: pd.Series,
        k_period: int = 14,
        d_period: int = 3
    ) -> Tuple[pd.Series, pd.Series]:
        """
        計算隨機指標 (Stochastic Oscillator, KD 指標)
        
        Parameters:
        -----------
        high : pd.Series
            最高價序列
        low : pd.Series
            最低價序列
        close : pd.Series
            收盤價序列
        k_period : int
            K 值週期
        d_period : int
            D 值週期
            
        Returns:
        --------
        Tuple[pd.Series, pd.Series]
            (K值, D值)
        """
        lowest_low = low.rolling(window=k_period).min()
        highest_high = high.rolling(window=k_period).max()
        
        k = 100 * (close - lowest_low) / (highest_high - lowest_low)
        d = k.rolling(window=d_period).mean()
        
        return k, d
    
    @staticmethod
    def add_all_indicators(df: pd.DataFrame) -> pd.DataFrame:
        """
        為資料框添加所有技術指標
        
        Parameters:
        -----------
        df : pd.DataFrame
            包含 OHLCV 資料的 DataFrame
            
        Returns:
        --------
        pd.DataFrame
            添加技術指標後的 DataFrame
        """
        df = df.copy()
        
        # 確保欄位名稱為小寫
        df.columns = df.columns.str.lower()
        
        # 趨勢指標
        short_period = TREND_FOLLOWING_PARAMS['short_ma_period']
        long_period = TREND_FOLLOWING_PARAMS['long_ma_period']
        ma_type = TREND_FOLLOWING_PARAMS['ma_type']
        
        df['ma_short'] = TechnicalIndicators.calculate_ma(
            df['close'], short_period, ma_type
        )
        df['ma_long'] = TechnicalIndicators.calculate_ma(
            df['close'], long_period, ma_type
        )
        
        # MACD
        macd, signal, histogram = TechnicalIndicators.calculate_macd(
            df['close'],
            TREND_FOLLOWING_PARAMS['macd_fast'],
            TREND_FOLLOWING_PARAMS['macd_slow'],
            TREND_FOLLOWING_PARAMS['macd_signal']
        )
        df['macd'] = macd
        df['macd_signal'] = signal
        df['macd_histogram'] = histogram
        
        # 均值回歸指標
        df['rsi'] = TechnicalIndicators.calculate_rsi(
            df['close'], 
            MEAN_REVERSION_PARAMS['rsi_period']
        )
        
        # 布林通道
        bb_upper, bb_middle, bb_lower = TechnicalIndicators.calculate_bollinger_bands(
            df['close'],
            MEAN_REVERSION_PARAMS['bb_period'],
            MEAN_REVERSION_PARAMS['bb_std']
        )
        df['bb_upper'] = bb_upper
        df['bb_middle'] = bb_middle
        df['bb_lower'] = bb_lower
        df['bb_width'] = (bb_upper - bb_lower) / bb_middle
        
        # 價格相對布林通道位置 (0-1)
        df['bb_position'] = (df['close'] - bb_lower) / (bb_upper - bb_lower)
        
        # 其他指標
        df['atr'] = TechnicalIndicators.calculate_atr(
            df['high'], df['low'], df['close'],
            INDICATOR_PARAMS['atr_period']
        )
        
        df['volume_ma'] = TechnicalIndicators.calculate_volume_ma(
            df['volume'],
            TREND_FOLLOWING_PARAMS['volume_ma_period']
        )
        
        df['obv'] = TechnicalIndicators.calculate_obv(df['close'], df['volume'])
        
        # KD 指標
        k, d = TechnicalIndicators.calculate_stochastic(
            df['high'], df['low'], df['close']
        )
        df['stoch_k'] = k
        df['stoch_d'] = d
        
        return df


def test_indicators():
    """測試技術指標計算"""
    print("=" * 60)
    print("測試技術指標計算功能")
    print("=" * 60)
    
    # 建立測試資料
    dates = pd.date_range('2023-01-01', periods=100, freq='D')
    np.random.seed(42)
    
    # 模擬股價資料
    close = 100 + np.cumsum(np.random.randn(100) * 2)
    high = close + np.random.rand(100) * 2
    low = close - np.random.rand(100) * 2
    volume = np.random.randint(1000000, 10000000, 100)
    
    df = pd.DataFrame({
        'close': close,
        'high': high,
        'low': low,
        'open': close,
        'volume': volume
    }, index=dates)
    
    # 添加所有指標
    df_with_indicators = TechnicalIndicators.add_all_indicators(df)
    
    print("\n添加的技術指標:")
    print(df_with_indicators.columns.tolist())
    
    print("\n最後 5 筆資料:")
    print(df_with_indicators.tail())
    
    print("\n✅ 測試完成！")


if __name__ == "__main__":
    test_indicators()
