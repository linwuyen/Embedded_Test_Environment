"""
台股半導體量化交易系統 - 主程式
Taiwan Semiconductor Quantitative Trading System - Main Program
"""

import pandas as pd
import numpy as np
from datetime import datetime
import sys
import os

# 加入 src 到路徑
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from config import BACKTEST_CONFIG, SEMICONDUCTOR_STOCKS
from src.data_loader import TaiwanStockDataLoader
from src.indicators import TechnicalIndicators
from src.strategies import StrategyManager
from src.backtester import Backtester
from src.visualizer import Visualizer


def main():
    """主程式"""
    print("=" * 80)
    print("台股半導體量化交易系統")
    print("Taiwan Semiconductor Quantitative Trading System")
    print("=" * 80)
    
    # ==================== 1. 資料載入 ====================
    print("\n【步驟 1】載入股票資料...")
    loader = TaiwanStockDataLoader()
    
    # 獲取處理後的資料
    data = loader.get_processed_data(
        start_date=BACKTEST_CONFIG['start_date'],
        end_date=BACKTEST_CONFIG['end_date']
    )
    
    # 顯示資料摘要
    print("\n資料摘要:")
    summary = loader.get_data_summary()
    print(summary.to_string(index=False))
    
    # ==================== 2. 計算技術指標 ====================
    print("\n【步驟 2】計算技術指標...")
    
    data_with_indicators = {}
    for symbol, df in data.items():
        df_indicators = TechnicalIndicators.add_all_indicators(df)
        data_with_indicators[symbol] = df_indicators
        print(f"  ✓ {SEMICONDUCTOR_STOCKS[symbol]} ({symbol}): 已添加技術指標")
    
    # ==================== 3. 生成交易訊號 ====================
    print("\n【步驟 3】生成交易訊號...")
    
    strategy_manager = StrategyManager()
    strategy_name = 'hybrid'  # 使用混合策略
    
    data_with_signals = {}
    for symbol, df in data_with_indicators.items():
        df_signals = strategy_manager.run_strategy(strategy_name, df)
        data_with_signals[symbol] = df_signals
        
        # 統計訊號數量
        buy_count = (df_signals['signal'] == 1).sum()
        sell_count = (df_signals['signal'] == -1).sum()
        print(f"  ✓ {SEMICONDUCTOR_STOCKS[symbol]} ({symbol}): "
              f"買入訊號 {buy_count} 個, 賣出訊號 {sell_count} 個")
    
    # ==================== 4. 執行回測 ====================
    print("\n【步驟 4】執行回測...")
    
    backtester = Backtester()
    all_results = {}
    
    for symbol, df in data_with_signals.items():
        print(f"\n回測 {SEMICONDUCTOR_STOCKS[symbol]} ({symbol})...")
        results = backtester.run_backtest(df)
        all_results[symbol] = results
        
        # 顯示績效指標
        metrics = results['metrics']
        print(f"  總報酬率: {metrics['總報酬率']:.2%}")
        print(f"  年化報酬率: {metrics['年化報酬率']:.2%}")
        print(f"  夏普比率: {metrics['夏普比率']:.2f}")
        print(f"  最大回撤: {metrics['最大回撤']:.2%}")
        print(f"  交易次數: {metrics['交易次數']:.0f}")
        print(f"  勝率: {metrics['勝率']:.2%}")
    
    # ==================== 5. 績效比較 ====================
    print("\n【步驟 5】績效比較...")
    
    comparison_data = []
    for symbol, results in all_results.items():
        metrics = results['metrics']
        comparison_data.append({
            '股票代碼': symbol,
            '股票名稱': SEMICONDUCTOR_STOCKS[symbol],
            '總報酬率': metrics['總報酬率'],
            '年化報酬率': metrics['年化報酬率'],
            '夏普比率': metrics['夏普比率'],
            '最大回撤': metrics['最大回撤'],
            '交易次數': metrics['交易次數'],
            '勝率': metrics['勝率']
        })
    
    comparison_df = pd.DataFrame(comparison_data)
    comparison_df = comparison_df.sort_values('總報酬率', ascending=False)
    
    print("\n績效排名:")
    print(comparison_df.to_string(index=False))
    
    # ==================== 6. 視覺化 ====================
    print("\n【步驟 6】生成視覺化圖表...")
    
    visualizer = Visualizer()
    
    # 選擇表現最好的股票進行詳細分析
    best_symbol = comparison_df.iloc[0]['股票代碼']
    best_name = comparison_df.iloc[0]['股票名稱']
    
    print(f"\n生成 {best_name} ({best_symbol}) 的詳細分析圖表...")
    
    best_results = all_results[best_symbol]
    best_data = data_with_signals[best_symbol]
    
    # 繪製資金曲線
    visualizer.plot_equity_curve(
        best_results['equity_curve'],
        title=f"{best_name} 資金曲線",
        save_name=f"{best_symbol.replace('.TW', '')}_equity_curve"
    )
    
    # 繪製交易訊號
    visualizer.plot_signals(
        best_data,
        title=f"{best_name} 交易訊號",
        save_name=f"{best_symbol.replace('.TW', '')}_signals"
    )
    
    # 繪製技術指標
    visualizer.plot_indicators(
        best_data,
        title=f"{best_name} 技術指標",
        save_name=f"{best_symbol.replace('.TW', '')}_indicators"
    )
    
    # 繪製回撤分析
    visualizer.plot_drawdown(
        best_results['equity_curve'],
        title=f"{best_name} 回撤分析",
        save_name=f"{best_symbol.replace('.TW', '')}_drawdown"
    )
    
    # 生成完整報告
    visualizer.generate_report(
        best_results['metrics'],
        best_results['equity_curve'],
        best_results['trades'],
        save_name=f"{best_symbol.replace('.TW', '')}_report"
    )
    
    # ==================== 7. 儲存結果 ====================
    print("\n【步驟 7】儲存結果...")
    
    # 儲存績效比較
    comparison_df.to_csv('results/performance_comparison.csv', index=False, encoding='utf-8-sig')
    print("  ✓ 績效比較已儲存至 results/performance_comparison.csv")
    
    # 儲存各股票的交易記錄
    for symbol, results in all_results.items():
        if len(results['trades']) > 0:
            filename = f"results/{symbol.replace('.TW', '')}_trades.csv"
            results['trades'].to_csv(filename, index=False, encoding='utf-8-sig')
            print(f"  ✓ {SEMICONDUCTOR_STOCKS[symbol]} 交易記錄已儲存至 {filename}")
    
    print("\n" + "=" * 80)
    print("✅ 回測完成！")
    print("=" * 80)
    print(f"\n最佳表現: {best_name} ({best_symbol})")
    print(f"總報酬率: {comparison_df.iloc[0]['總報酬率']:.2%}")
    print(f"年化報酬率: {comparison_df.iloc[0]['年化報酬率']:.2%}")
    print(f"夏普比率: {comparison_df.iloc[0]['夏普比率']:.2f}")
    print("\n所有圖表已儲存至 results/ 目錄")
    print("=" * 80)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n程式已中斷")
    except Exception as e:
        print(f"\n\n錯誤: {str(e)}")
        import traceback
        traceback.print_exc()
