"""
回測引擎模組
Backtesting Engine Module
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Tuple, Optional
from datetime import datetime
import sys
import os

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from config import (
    BACKTEST_CONFIG,
    TRANSACTION_COSTS,
    RISK_MANAGEMENT
)


class Backtester:
    """回測引擎類別"""
    
    def __init__(
        self,
        initial_capital: float = None,
        commission_rate: float = None,
        tax_rate: float = None,
        slippage: float = None
    ):
        """
        初始化回測引擎
        
        Parameters:
        -----------
        initial_capital : float
            初始資金
        commission_rate : float
            手續費率
        tax_rate : float
            交易稅率
        slippage : float
            滑價率
        """
        self.initial_capital = initial_capital or BACKTEST_CONFIG['initial_capital']
        self.commission_rate = commission_rate or TRANSACTION_COSTS['commission_rate']
        self.tax_rate = tax_rate or TRANSACTION_COSTS['tax_rate']
        self.slippage = slippage or TRANSACTION_COSTS['slippage']
        
        self.results = None
        self.trades = []
        self.equity_curve = None
    
    def calculate_transaction_cost(
        self, 
        price: float, 
        shares: int, 
        is_buy: bool
    ) -> float:
        """
        計算交易成本
        
        Parameters:
        -----------
        price : float
            成交價格
        shares : int
            股數
        is_buy : bool
            是否為買入
            
        Returns:
        --------
        float
            交易成本
        """
        transaction_value = price * shares
        
        # 手續費 (買賣都收)
        commission = transaction_value * self.commission_rate
        
        # 證券交易稅 (只有賣出收)
        tax = transaction_value * self.tax_rate if not is_buy else 0
        
        # 滑價
        slippage_cost = transaction_value * self.slippage
        
        return commission + tax + slippage_cost
    
    def run_backtest(
        self, 
        df: pd.DataFrame,
        position_size: float = None
    ) -> Dict:
        """
        執行回測
        
        Parameters:
        -----------
        df : pd.DataFrame
            包含訊號的資料
        position_size : float
            單筆投入比例 (0-1)
            
        Returns:
        --------
        Dict
            回測結果
        """
        df = df.copy()
        position_size = position_size or BACKTEST_CONFIG['position_size']
        
        # 初始化
        cash = self.initial_capital
        shares = 0
        equity = self.initial_capital
        
        # 記錄每日資產
        equity_curve = []
        trades = []
        
        # 風險管理參數
        stop_loss = RISK_MANAGEMENT['stop_loss']
        take_profit = RISK_MANAGEMENT['take_profit']
        entry_price = 0
        
        for idx, row in df.iterrows():
            current_price = row['close']
            signal = row['signal']
            
            # 計算當前資產
            equity = cash + shares * current_price
            
            # 停損停利檢查
            if shares > 0 and entry_price > 0:
                returns = (current_price - entry_price) / entry_price
                
                # 停損
                if returns <= -stop_loss:
                    signal = -1  # 強制賣出
                    print(f"觸發停損: {idx.date()}, 虧損 {returns:.2%}")
                
                # 停利
                elif returns >= take_profit:
                    signal = -1  # 強制賣出
                    print(f"觸發停利: {idx.date()}, 獲利 {returns:.2%}")
            
            # 處理交易訊號
            if signal == 1 and shares == 0:  # 買入訊號且目前無持倉
                # 計算可買股數
                available_cash = cash * position_size
                buy_price = current_price * (1 + self.slippage)
                max_shares = int(available_cash / buy_price / 1000) * 1000  # 台股以千股為單位
                
                if max_shares > 0:
                    # 計算交易成本
                    cost = self.calculate_transaction_cost(buy_price, max_shares, True)
                    total_cost = buy_price * max_shares + cost
                    
                    if total_cost <= cash:
                        shares = max_shares
                        cash -= total_cost
                        entry_price = buy_price
                        
                        trades.append({
                            'date': idx,
                            'type': 'BUY',
                            'price': buy_price,
                            'shares': shares,
                            'cost': cost,
                            'cash': cash,
                            'equity': equity
                        })
            
            elif signal == -1 and shares > 0:  # 賣出訊號且目前有持倉
                # 賣出所有股票
                sell_price = current_price * (1 - self.slippage)
                cost = self.calculate_transaction_cost(sell_price, shares, False)
                proceeds = sell_price * shares - cost
                
                # 計算報酬
                trade_return = (sell_price - entry_price) / entry_price
                
                cash += proceeds
                
                trades.append({
                    'date': idx,
                    'type': 'SELL',
                    'price': sell_price,
                    'shares': shares,
                    'cost': cost,
                    'cash': cash,
                    'equity': equity,
                    'return': trade_return
                })
                
                shares = 0
                entry_price = 0
            
            # 記錄每日資產
            equity_curve.append({
                'date': idx,
                'cash': cash,
                'shares': shares,
                'price': current_price,
                'equity': cash + shares * current_price
            })
        
        # 儲存結果
        self.equity_curve = pd.DataFrame(equity_curve).set_index('date')
        self.trades = pd.DataFrame(trades)
        
        # 計算績效指標
        metrics = self.calculate_metrics()
        
        return {
            'equity_curve': self.equity_curve,
            'trades': self.trades,
            'metrics': metrics
        }
    
    def calculate_metrics(self) -> Dict:
        """
        計算績效指標
        
        Returns:
        --------
        Dict
            績效指標字典
        """
        if self.equity_curve is None or len(self.equity_curve) == 0:
            return {}
        
        # 基本統計
        final_equity = self.equity_curve['equity'].iloc[-1]
        total_return = (final_equity - self.initial_capital) / self.initial_capital
        
        # 計算日報酬率
        daily_returns = self.equity_curve['equity'].pct_change().dropna()
        
        # 年化報酬率
        trading_days = len(self.equity_curve)
        years = trading_days / 252
        annual_return = (1 + total_return) ** (1 / years) - 1 if years > 0 else 0
        
        # 波動率
        volatility = daily_returns.std() * np.sqrt(252)
        
        # 夏普比率 (假設無風險利率為 1%)
        risk_free_rate = 0.01
        sharpe_ratio = (annual_return - risk_free_rate) / volatility if volatility > 0 else 0
        
        # 最大回撤
        cumulative_returns = (1 + daily_returns).cumprod()
        running_max = cumulative_returns.expanding().max()
        drawdown = (cumulative_returns - running_max) / running_max
        max_drawdown = drawdown.min()
        
        # 交易統計
        if len(self.trades) > 0:
            buy_trades = self.trades[self.trades['type'] == 'BUY']
            sell_trades = self.trades[self.trades['type'] == 'SELL']
            
            num_trades = len(sell_trades)
            
            if 'return' in sell_trades.columns and len(sell_trades) > 0:
                winning_trades = sell_trades[sell_trades['return'] > 0]
                losing_trades = sell_trades[sell_trades['return'] <= 0]
                
                win_rate = len(winning_trades) / num_trades if num_trades > 0 else 0
                avg_win = winning_trades['return'].mean() if len(winning_trades) > 0 else 0
                avg_loss = losing_trades['return'].mean() if len(losing_trades) > 0 else 0
                
                # 獲利因子
                total_wins = (winning_trades['return'] * winning_trades['shares'] * winning_trades['price']).sum()
                total_losses = abs((losing_trades['return'] * losing_trades['shares'] * losing_trades['price']).sum())
                profit_factor = total_wins / total_losses if total_losses > 0 else 0
            else:
                win_rate = avg_win = avg_loss = profit_factor = 0
        else:
            num_trades = win_rate = avg_win = avg_loss = profit_factor = 0
        
        return {
            '初始資金': self.initial_capital,
            '最終資金': final_equity,
            '總報酬率': total_return,
            '年化報酬率': annual_return,
            '波動率': volatility,
            '夏普比率': sharpe_ratio,
            '最大回撤': max_drawdown,
            '交易次數': num_trades,
            '勝率': win_rate,
            '平均獲利': avg_win,
            '平均虧損': avg_loss,
            '獲利因子': profit_factor,
            '交易天數': trading_days
        }
    
    def get_metrics_summary(self) -> pd.DataFrame:
        """獲取績效指標摘要表"""
        metrics = self.calculate_metrics()
        
        summary = pd.DataFrame([
            {'指標': '初始資金', '數值': f"NT$ {metrics['初始資金']:,.0f}"},
            {'指標': '最終資金', '數值': f"NT$ {metrics['最終資金']:,.0f}"},
            {'指標': '總報酬率', '數值': f"{metrics['總報酬率']:.2%}"},
            {'指標': '年化報酬率', '數值': f"{metrics['年化報酬率']:.2%}"},
            {'指標': '波動率', '數值': f"{metrics['波動率']:.2%}"},
            {'指標': '夏普比率', '數值': f"{metrics['夏普比率']:.2f}"},
            {'指標': '最大回撤', '數值': f"{metrics['最大回撤']:.2%}"},
            {'指標': '交易次數', '數值': f"{metrics['交易次數']:.0f}"},
            {'指標': '勝率', '數值': f"{metrics['勝率']:.2%}"},
            {'指標': '平均獲利', '數值': f"{metrics['平均獲利']:.2%}"},
            {'指標': '平均虧損', '數值': f"{metrics['平均虧損']:.2%}"},
            {'指標': '獲利因子', '數值': f"{metrics['獲利因子']:.2f}"},
        ])
        
        return summary


def test_backtester():
    """測試回測引擎"""
    print("=" * 60)
    print("測試回測引擎功能")
    print("=" * 60)
    
    # 建立測試資料
    dates = pd.date_range('2023-01-01', periods=100, freq='D')
    np.random.seed(42)
    
    # 模擬股價與訊號
    close = 100 + np.cumsum(np.random.randn(100) * 2)
    signal = np.zeros(100)
    signal[10] = 1  # 買入
    signal[30] = -1  # 賣出
    signal[50] = 1  # 買入
    signal[80] = -1  # 賣出
    
    df = pd.DataFrame({
        'close': close,
        'signal': signal
    }, index=dates)
    
    # 執行回測
    backtester = Backtester(initial_capital=1000000)
    results = backtester.run_backtest(df)
    
    print("\n績效指標:")
    print(backtester.get_metrics_summary().to_string(index=False))
    
    print("\n\n交易記錄:")
    if len(backtester.trades) > 0:
        print(backtester.trades.to_string(index=False))
    else:
        print("無交易記錄")
    
    print("\n✅ 測試完成！")


if __name__ == "__main__":
    test_backtester()
