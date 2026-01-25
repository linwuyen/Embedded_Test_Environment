
import pandas as pd
import sys
import os

# Add project root to path
sys.path.append(os.getcwd())

from config import BACKTEST_CONFIG, SEMICONDUCTOR_STOCKS
from src.data_loader import TaiwanStockDataLoader
from src.indicators import TechnicalIndicators
from src.strategies import StrategyManager

def debug_strategies():
    print("=" * 80)
    print("DEBUG: Strategy Signal Generation Analysis")
    print("=" * 80)

    # 1. Load Data
    print("\n1. Loading Data...")
    loader = TaiwanStockDataLoader()
    # Use a smaller range if needed, or the default config
    data = loader.get_processed_data() 
    
    if not data:
        print("ERROR: No data loaded.")
        return

    print(f"Loaded {len(data)} stocks.")
    
    # Check one major stock (e.g., TSMC 2330.TW)
    target_stock = '2330.TW'
    if target_stock not in data:
        target_stock = list(data.keys())[0]
    
    print(f"\nAnalyzing stock: {target_stock} ({SEMICONDUCTOR_STOCKS.get(target_stock, 'Unknown')})")
    df = data[target_stock]
    print(f"Data range: {df.index.min()} to {df.index.max()}")
    print(f"Total rows: {len(df)}")

    # 2. Add Indicators
    print("\n2. Calculating Indicators...")
    df = TechnicalIndicators.add_all_indicators(df)
    
    # Check if indicators are valid (not all NaN)
    print("Indicator Check (NaN counts):")
    print(df[['ma_short', 'ma_long', 'macd', 'rsi', 'bb_upper']].isna().sum())

    # 3. Test Individual Strategies
    manager = StrategyManager()
    
    print("\n3. Testing Strategies...")
    
    # 3.1 Trend Following
    print("\n--- Trend Following Strategy ---")
    trend_strategy = manager.strategies['trend']
    trend_signals = trend_strategy.generate_signals(df)
    trend_summary = trend_strategy.get_signal_summary(trend_signals)
    print(trend_summary)
    
    # Debug Trend Conditions
    if trend_summary['buy_signals'] == 0:
        print("DEBUG TREND: Why no buy signals?")
        # Check conditions roughly
        golden_cross_count = (df['ma_short'] > df['ma_long']) & (df['ma_short'].shift(1) <= df['ma_long'].shift(1))
        print(f"  Golden Crosses: {golden_cross_count.sum()}")
        macd_bullish_count = (df['macd'] > df['macd_signal']).sum()
        print(f"  MACD Bullish Days: {macd_bullish_count}")
        volume_spike_count = (df['volume'] > df['volume_ma'] * 1.2).sum()
        print(f"  Volume Spikes (>1.2x MA): {volume_spike_count}")
        
    # 3.2 Mean Reversion
    print("\n--- Mean Reversion Strategy ---")
    mean_strategy = manager.strategies['mean_reversion']
    mean_signals = mean_strategy.generate_signals(df)
    mean_summary = mean_strategy.get_signal_summary(mean_signals)
    print(mean_summary)
    
    if mean_summary['buy_signals'] == 0:
        print("DEBUG MEAN: Why no buy signals?")
        rsi_oversold = (df['rsi'] < 30).sum()
        print(f"  RSI < 30 Days: {rsi_oversold}")
        bb_lower_touch = (df['bb_position'] < 0.2).sum()
        print(f"  BB Lower Touch (<0.2) Days: {bb_lower_touch}")

    # 3.3 Hybrid Strategy
    print("\n--- Hybrid Strategy ---")
    hybrid_strategy = manager.strategies['hybrid']
    hybrid_signals = hybrid_strategy.generate_signals(df)
    hybrid_summary = hybrid_strategy.get_signal_summary(hybrid_signals)
    print(hybrid_summary)
    
    if hybrid_summary['buy_signals'] == 0:
        print("DEBUG HYBRID: Why no buy signals?")
        print("  Hybrid logic requires Trend=1 AND Mean=1, or Trend=1 AND Mean>=0 AND RSI<50")
        
        df_debug = df.copy()
        df_debug['trend_sig'] = trend_signals['signal']
        df_debug['mean_sig'] = mean_signals['signal']
        
        # Check overlap
        both_agree = ((df_debug['trend_sig'] == 1) & (df_debug['mean_sig'] == 1)).sum()
        print(f"  Both Agree (Trend=1 & Mean=1): {both_agree}")
        
        soft_confirm = ((df_debug['trend_sig'] == 1) & (df_debug['mean_sig'] >= 0) & (df_debug['rsi'] < 50)).sum()
        print(f"  Soft Confirm (Trend=1 & Mean>=0 & RSI<50): {soft_confirm}")

if __name__ == "__main__":
    debug_strategies()
