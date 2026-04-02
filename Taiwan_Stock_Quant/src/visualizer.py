"""
視覺化模組
Visualization Module
"""

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np
from pathlib import Path
from typing import Dict, List, Optional, Tuple
import sys
import os

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from config import VISUALIZATION_CONFIG, DATA_PATHS

# 設定中文字體
# 設定樣式
sns.set_style("whitegrid")

# 設定中文字體 (需在 sns.set_style 之後設定，以免被覆蓋)
import platform
if platform.system() == 'Windows':
    plt.rcParams['font.sans-serif'] = ['Microsoft JhengHei', 'Microsoft JhengHei UI', 'PMingLiU', 'SimHei', 'Arial Unicode MS']
else:
    plt.rcParams['font.sans-serif'] = ['Arial Unicode MS', 'Microsoft JhengHei', 'SimHei']
plt.rcParams['axes.unicode_minus'] = False


class Visualizer:
    """視覺化類別"""
    
    def __init__(self, save_dir: str = None):
        """
        初始化視覺化器
        
        Parameters:
        -----------
        save_dir : str
            圖表儲存目錄
        """
        self.save_dir = Path(save_dir or DATA_PATHS['results'])
        self.save_dir.mkdir(parents=True, exist_ok=True)
        
        self.fig_size = VISUALIZATION_CONFIG['figure_size']
        self.dpi = VISUALIZATION_CONFIG['dpi']
        self.save_format = VISUALIZATION_CONFIG['save_format']

    def show(self):
        """顯示所有圖表"""
        plt.show()
    
    def plot_equity_curve(
        self,
        equity_curve: pd.DataFrame,
        title: str = "資金曲線",
        save_name: str = None
    ):
        """
        繪製資金曲線圖
        
        Parameters:
        -----------
        equity_curve : pd.DataFrame
            資金曲線資料
        title : str
            圖表標題
        save_name : str
            儲存檔名
        """
        fig, ax = plt.subplots(figsize=self.fig_size)
        
        ax.plot(equity_curve.index, equity_curve['equity'], 
                linewidth=2, label='總資產', color='#2E86AB')
        ax.fill_between(equity_curve.index, equity_curve['equity'], 
                        alpha=0.3, color='#2E86AB')
        
        ax.set_title(title, fontsize=16, fontweight='bold', pad=20)
        ax.set_xlabel('日期', fontsize=12)
        ax.set_ylabel('資產 (NT$)', fontsize=12)
        ax.legend(fontsize=10)
        ax.grid(True, alpha=0.3)
        
        # 格式化 Y 軸
        ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'${x:,.0f}'))
        
        plt.tight_layout()
        
        if save_name:
            plt.savefig(self.save_dir / f"{save_name}.{self.save_format}", 
                       dpi=self.dpi, bbox_inches='tight')
    
    def plot_signals(
        self,
        df: pd.DataFrame,
        title: str = "交易訊號",
        save_name: str = None
    ):
        """
        繪製價格與交易訊號
        
        Parameters:
        -----------
        df : pd.DataFrame
            包含價格與訊號的資料
        title : str
            圖表標題
        save_name : str
            儲存檔名
        """
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(15, 10), 
                                        gridspec_kw={'height_ratios': [3, 1]})
        
        # 價格與訊號
        ax1.plot(df.index, df['close'], linewidth=1.5, 
                label='收盤價', color='black', alpha=0.7)
        
        # 買入訊號
        buy_signals = df[df['signal'] == 1]
        ax1.scatter(buy_signals.index, buy_signals['close'], 
                   marker='^', color='green', s=100, label='買入', zorder=5)
        
        # 賣出訊號
        sell_signals = df[df['signal'] == -1]
        ax1.scatter(sell_signals.index, sell_signals['close'], 
                   marker='v', color='red', s=100, label='賣出', zorder=5)
        
        # 移動平均線
        if 'ma_short' in df.columns:
            ax1.plot(df.index, df['ma_short'], linewidth=1, 
                    label='短期均線', color='orange', alpha=0.7)
        if 'ma_long' in df.columns:
            ax1.plot(df.index, df['ma_long'], linewidth=1, 
                    label='長期均線', color='blue', alpha=0.7)
        
        ax1.set_title(title, fontsize=16, fontweight='bold', pad=20)
        ax1.set_ylabel('價格 (NT$)', fontsize=12)
        ax1.legend(loc='best', fontsize=10)
        ax1.grid(True, alpha=0.3)
        
        # 成交量
        colors = ['green' if df['close'].iloc[i] >= df['open'].iloc[i] 
                 else 'red' for i in range(len(df))]
        ax2.bar(df.index, df['volume'], color=colors, alpha=0.5)
        ax2.set_ylabel('成交量', fontsize=12)
        ax2.set_xlabel('日期', fontsize=12)
        ax2.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_name:
            plt.savefig(self.save_dir / f"{save_name}.{self.save_format}", 
                       dpi=self.dpi, bbox_inches='tight')
    
    def plot_indicators(
        self,
        df: pd.DataFrame,
        title: str = "技術指標",
        save_name: str = None
    ):
        """
        繪製技術指標圖表
        
        Parameters:
        -----------
        df : pd.DataFrame
            包含技術指標的資料
        title : str
            圖表標題
        save_name : str
            儲存檔名
        """
        fig, axes = plt.subplots(4, 1, figsize=(15, 12), sharex=True)
        
        # 1. 價格與布林通道
        axes[0].plot(df.index, df['close'], linewidth=1.5, 
                    label='收盤價', color='black')
        if 'bb_upper' in df.columns:
            axes[0].plot(df.index, df['bb_upper'], linewidth=1, 
                        label='布林上軌', color='red', linestyle='--', alpha=0.7)
            axes[0].plot(df.index, df['bb_middle'], linewidth=1, 
                        label='布林中軌', color='blue', linestyle='--', alpha=0.7)
            axes[0].plot(df.index, df['bb_lower'], linewidth=1, 
                        label='布林下軌', color='green', linestyle='--', alpha=0.7)
            axes[0].fill_between(df.index, df['bb_upper'], df['bb_lower'], 
                                alpha=0.1, color='gray')
        axes[0].set_ylabel('價格', fontsize=10)
        axes[0].legend(loc='best', fontsize=8)
        axes[0].set_title(title, fontsize=14, fontweight='bold')
        axes[0].grid(True, alpha=0.3)
        
        # 2. MACD
        if 'macd' in df.columns:
            axes[1].plot(df.index, df['macd'], linewidth=1.5, 
                        label='MACD', color='blue')
            axes[1].plot(df.index, df['macd_signal'], linewidth=1.5, 
                        label='訊號線', color='red')
            axes[1].bar(df.index, df['macd_histogram'], 
                       label='柱狀圖', color='gray', alpha=0.3)
            axes[1].axhline(y=0, color='black', linestyle='-', linewidth=0.5)
            axes[1].set_ylabel('MACD', fontsize=10)
            axes[1].legend(loc='best', fontsize=8)
            axes[1].grid(True, alpha=0.3)
        
        # 3. RSI
        if 'rsi' in df.columns:
            axes[2].plot(df.index, df['rsi'], linewidth=1.5, 
                        label='RSI', color='purple')
            axes[2].axhline(y=70, color='red', linestyle='--', 
                           linewidth=1, alpha=0.7, label='超買')
            axes[2].axhline(y=30, color='green', linestyle='--', 
                           linewidth=1, alpha=0.7, label='超賣')
            axes[2].fill_between(df.index, 30, 70, alpha=0.1, color='gray')
            axes[2].set_ylabel('RSI', fontsize=10)
            axes[2].set_ylim(0, 100)
            axes[2].legend(loc='best', fontsize=8)
            axes[2].grid(True, alpha=0.3)
        
        # 4. 成交量
        colors = ['green' if df['close'].iloc[i] >= df['open'].iloc[i] 
                 else 'red' for i in range(len(df))]
        axes[3].bar(df.index, df['volume'], color=colors, alpha=0.5)
        if 'volume_ma' in df.columns:
            axes[3].plot(df.index, df['volume_ma'], linewidth=1.5, 
                        label='成交量均線', color='orange')
        axes[3].set_ylabel('成交量', fontsize=10)
        axes[3].set_xlabel('日期', fontsize=10)
        axes[3].legend(loc='best', fontsize=8)
        axes[3].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_name:
            plt.savefig(self.save_dir / f"{save_name}.{self.save_format}", 
                       dpi=self.dpi, bbox_inches='tight')
    
    def plot_drawdown(
        self,
        equity_curve: pd.DataFrame,
        title: str = "回撤分析",
        save_name: str = None
    ):
        """
        繪製回撤分析圖
        
        Parameters:
        -----------
        equity_curve : pd.DataFrame
            資金曲線資料
        title : str
            圖表標題
        save_name : str
            儲存檔名
        """
        # 計算回撤
        equity = equity_curve['equity']
        running_max = equity.expanding().max()
        drawdown = (equity - running_max) / running_max
        
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=self.fig_size, sharex=True)
        
        # 資金曲線
        ax1.plot(equity.index, equity, linewidth=2, 
                label='總資產', color='#2E86AB')
        ax1.plot(running_max.index, running_max, linewidth=1, 
                label='歷史最高', color='red', linestyle='--', alpha=0.7)
        ax1.set_ylabel('資產 (NT$)', fontsize=12)
        ax1.legend(loc='best', fontsize=10)
        ax1.set_title(title, fontsize=16, fontweight='bold', pad=20)
        ax1.grid(True, alpha=0.3)
        ax1.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'${x:,.0f}'))
        
        # 回撤
        ax2.fill_between(drawdown.index, drawdown, 0, 
                        color='red', alpha=0.3, label='回撤')
        ax2.plot(drawdown.index, drawdown, linewidth=1.5, color='red')
        ax2.set_ylabel('回撤 (%)', fontsize=12)
        ax2.set_xlabel('日期', fontsize=12)
        ax2.legend(loc='best', fontsize=10)
        ax2.grid(True, alpha=0.3)
        ax2.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:.1%}'))
        
        plt.tight_layout()
        
        if save_name:
            plt.savefig(self.save_dir / f"{save_name}.{self.save_format}", 
                       dpi=self.dpi, bbox_inches='tight')
    
    def generate_report(
        self,
        metrics: Dict,
        equity_curve: pd.DataFrame,
        trades: pd.DataFrame,
        save_name: str = "backtest_report"
    ):
        """
        生成完整回測報告
        
        Parameters:
        -----------
        metrics : Dict
            績效指標
        equity_curve : pd.DataFrame
            資金曲線
        trades : pd.DataFrame
            交易記錄
        save_name : str
            報告檔名
        """
        fig = plt.figure(figsize=(16, 12))
        gs = fig.add_gridspec(3, 2, hspace=0.3, wspace=0.3)
        
        # 1. 資金曲線
        ax1 = fig.add_subplot(gs[0, :])
        ax1.plot(equity_curve.index, equity_curve['equity'], 
                linewidth=2, color='#2E86AB')
        ax1.fill_between(equity_curve.index, equity_curve['equity'], 
                        alpha=0.3, color='#2E86AB')
        ax1.set_title('資金曲線', fontsize=14, fontweight='bold')
        ax1.set_ylabel('資產 (NT$)', fontsize=10)
        ax1.grid(True, alpha=0.3)
        ax1.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'${x:,.0f}'))
        
        # 2. 月報酬率
        ax2 = fig.add_subplot(gs[1, 0])
        monthly_returns = equity_curve['equity'].resample('M').last().pct_change()
        colors = ['green' if x > 0 else 'red' for x in monthly_returns]
        ax2.bar(range(len(monthly_returns)), monthly_returns, color=colors, alpha=0.7)
        ax2.set_title('月報酬率', fontsize=12, fontweight='bold')
        ax2.set_ylabel('報酬率', fontsize=10)
        ax2.axhline(y=0, color='black', linestyle='-', linewidth=0.5)
        ax2.grid(True, alpha=0.3)
        ax2.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:.1%}'))
        
        # 3. 績效指標表
        ax3 = fig.add_subplot(gs[1, 1])
        ax3.axis('off')
        
        metrics_text = f"""
        績效指標摘要
        ─────────────────────
        總報酬率: {metrics['總報酬率']:.2%}
        年化報酬率: {metrics['年化報酬率']:.2%}
        夏普比率: {metrics['夏普比率']:.2f}
        最大回撤: {metrics['最大回撤']:.2%}
        
        交易統計
        ─────────────────────
        交易次數: {metrics['交易次數']:.0f}
        勝率: {metrics['勝率']:.2%}
        平均獲利: {metrics['平均獲利']:.2%}
        平均虧損: {metrics['平均虧損']:.2%}
        獲利因子: {metrics['獲利因子']:.2f}
        """
        
        ax3.text(0.1, 0.5, metrics_text, fontsize=11, 
                verticalalignment='center')
        
        # 4. 回撤圖
        ax4 = fig.add_subplot(gs[2, :])
        equity = equity_curve['equity']
        running_max = equity.expanding().max()
        drawdown = (equity - running_max) / running_max
        
        ax4.fill_between(drawdown.index, drawdown, 0, 
                        color='red', alpha=0.3)
        ax4.plot(drawdown.index, drawdown, linewidth=1.5, color='red')
        ax4.set_title('回撤分析', fontsize=12, fontweight='bold')
        ax4.set_ylabel('回撤 (%)', fontsize=10)
        ax4.set_xlabel('日期', fontsize=10)
        ax4.grid(True, alpha=0.3)
        ax4.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{x:.1%}'))
        
        plt.suptitle('回測報告', fontsize=18, fontweight='bold', y=0.995)
        
        # 儲存報告
        plt.savefig(self.save_dir / f"{save_name}.{self.save_format}", 
                   dpi=self.dpi, bbox_inches='tight')


def test_visualizer():
    """測試視覺化功能"""
    print("=" * 60)
    print("測試視覺化功能")
    print("=" * 60)
    
    # 建立測試資料
    dates = pd.date_range('2023-01-01', periods=100, freq='D')
    np.random.seed(42)
    
    equity = 1000000 + np.cumsum(np.random.randn(100) * 10000)
    
    equity_curve = pd.DataFrame({
        'equity': equity,
        'cash': equity * 0.5,
        'shares': 1000,
        'price': 100
    }, index=dates)
    
    visualizer = Visualizer()
    
    print("\n繪製資金曲線...")
    visualizer.plot_equity_curve(equity_curve, save_name="test_equity_curve")
    
    visualizer.show()
    
    print("\n✅ 測試完成！")


if __name__ == "__main__":
    test_visualizer()
