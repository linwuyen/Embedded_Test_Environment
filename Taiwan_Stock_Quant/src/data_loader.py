"""
資料獲取與處理模組
Data Loader Module for Taiwan Stock Market
"""

import yfinance as yf
import pandas as pd
import numpy as np
from datetime import datetime, timedelta
from pathlib import Path
import pickle
from typing import Dict, List, Optional, Union
from tqdm import tqdm
import sys
import os

# 加入父目錄到路徑以便導入 config
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from config import SEMICONDUCTOR_STOCKS, BACKTEST_CONFIG, DATA_PATHS


class TaiwanStockDataLoader:
    """台股資料下載與處理類別"""
    
    def __init__(self, stock_dict: Dict[str, str] = None):
        """
        初始化資料載入器
        
        Parameters:
        -----------
        stock_dict : Dict[str, str]
            股票代碼與名稱的字典，預設使用 config 中的半導體股票清單
        """
        self.stock_dict = stock_dict or SEMICONDUCTOR_STOCKS
        self.data = {}
        self.raw_data_path = Path(DATA_PATHS['raw_data'])
        self.processed_data_path = Path(DATA_PATHS['processed_data'])
        
        # 建立資料目錄
        self.raw_data_path.mkdir(parents=True, exist_ok=True)
        self.processed_data_path.mkdir(parents=True, exist_ok=True)
    
    def download_stock_data(
        self, 
        start_date: str = None, 
        end_date: str = None,
        force_download: bool = False
    ) -> Dict[str, pd.DataFrame]:
        """
        下載台股歷史資料
        
        Parameters:
        -----------
        start_date : str
            起始日期，格式: 'YYYY-MM-DD'
        end_date : str
            結束日期，格式: 'YYYY-MM-DD'
        force_download : bool
            是否強制重新下載（忽略快取）
            
        Returns:
        --------
        Dict[str, pd.DataFrame]
            股票代碼與資料的字典
        """
        start_date = start_date or BACKTEST_CONFIG['start_date']
        end_date = end_date or BACKTEST_CONFIG['end_date']
        
        print(f"📊 開始下載台股資料: {start_date} 至 {end_date}")
        print(f"📈 股票數量: {len(self.stock_dict)}")
        
        for symbol, name in tqdm(self.stock_dict.items(), desc="下載進度"):
            cache_file = self.raw_data_path / f"{symbol.replace('.TW', '')}_{start_date}_{end_date}.pkl"
            
            # 檢查快取
            if not force_download and cache_file.exists():
                print(f"  ✓ {name} ({symbol}): 使用快取資料")
                with open(cache_file, 'rb') as f:
                    self.data[symbol] = pickle.load(f)
                continue
            
            try:
                # 下載資料
                stock = yf.Ticker(symbol)
                df = stock.history(start=start_date, end=end_date)
                
                if df.empty:
                    print(f"  ✗ {name} ({symbol}): 無資料")
                    continue
                
                # 重新命名欄位為中文（可選）
                df.columns = df.columns.str.lower()
                
                self.data[symbol] = df
                
                # 儲存快取
                with open(cache_file, 'wb') as f:
                    pickle.dump(df, f)
                
                print(f"  ✓ {name} ({symbol}): 下載 {len(df)} 筆資料")
                
            except Exception as e:
                print(f"  ✗ {name} ({symbol}): 下載失敗 - {str(e)}")
        
        print(f"\n✅ 資料下載完成！成功下載 {len(self.data)} 檔股票")
        return self.data
    
    def preprocess_data(self, df: pd.DataFrame) -> pd.DataFrame:
        """
        資料預處理
        
        Parameters:
        -----------
        df : pd.DataFrame
            原始股票資料
            
        Returns:
        --------
        pd.DataFrame
            處理後的資料
        """
        df = df.copy()
        
        # 1. 移除缺失值
        df = df.dropna()
        
        # 2. 確保欄位名稱一致
        df.columns = df.columns.str.lower()
        
        # 3. 計算日報酬率
        df['returns'] = df['close'].pct_change()
        
        # 4. 計算對數報酬率
        df['log_returns'] = np.log(df['close'] / df['close'].shift(1))
        
        # 5. 移除異常值（報酬率超過 ±20% 視為異常）
        df = df[(df['returns'].abs() <= 0.20) | df['returns'].isna()]
        
        # 6. 重設索引
        df = df.reset_index()
        if 'Date' in df.columns:
            df['date'] = pd.to_datetime(df['Date'])
            df = df.drop('Date', axis=1)
        elif 'index' in df.columns:
            df['date'] = pd.to_datetime(df['index'])
            df = df.drop('index', axis=1)
        
        df = df.set_index('date')
        
        return df
    
    def get_processed_data(
        self, 
        start_date: str = None, 
        end_date: str = None,
        force_download: bool = False
    ) -> Dict[str, pd.DataFrame]:
        """
        獲取處理後的資料
        
        Parameters:
        -----------
        start_date : str
            起始日期
        end_date : str
            結束日期
        force_download : bool
            是否強制重新下載
            
        Returns:
        --------
        Dict[str, pd.DataFrame]
            處理後的股票資料字典
        """
        # 下載原始資料
        if not self.data or force_download:
            self.download_stock_data(start_date, end_date, force_download)
        
        # 預處理每檔股票
        processed_data = {}
        print("\n🔧 開始資料預處理...")
        
        for symbol, df in tqdm(self.data.items(), desc="處理進度"):
            try:
                processed_df = self.preprocess_data(df)
                processed_data[symbol] = processed_df
                
                # 儲存處理後的資料
                save_path = self.processed_data_path / f"{symbol.replace('.TW', '')}_processed.pkl"
                with open(save_path, 'wb') as f:
                    pickle.dump(processed_df, f)
                    
            except Exception as e:
                print(f"  ✗ {symbol}: 處理失敗 - {str(e)}")
        
        print(f"✅ 資料預處理完成！成功處理 {len(processed_data)} 檔股票\n")
        return processed_data
    
    def load_processed_data(self, symbol: str) -> Optional[pd.DataFrame]:
        """
        載入已處理的資料
        
        Parameters:
        -----------
        symbol : str
            股票代碼
            
        Returns:
        --------
        pd.DataFrame or None
            處理後的資料，若不存在則返回 None
        """
        file_path = self.processed_data_path / f"{symbol.replace('.TW', '')}_processed.pkl"
        
        if file_path.exists():
            with open(file_path, 'rb') as f:
                return pickle.load(f)
        return None
    
    def get_stock_info(self, symbol: str) -> Dict:
        """
        獲取股票基本資訊
        
        Parameters:
        -----------
        symbol : str
            股票代碼
            
        Returns:
        --------
        Dict
            股票資訊字典
        """
        try:
            stock = yf.Ticker(symbol)
            info = stock.info
            return {
                'symbol': symbol,
                'name': self.stock_dict.get(symbol, 'Unknown'),
                'sector': info.get('sector', 'N/A'),
                'industry': info.get('industry', 'N/A'),
                'market_cap': info.get('marketCap', 'N/A'),
            }
        except Exception as e:
            print(f"獲取 {symbol} 資訊失敗: {str(e)}")
            return {}
    
    def get_data_summary(self) -> pd.DataFrame:
        """
        獲取資料摘要統計
        
        Returns:
        --------
        pd.DataFrame
            資料摘要表
        """
        summary_data = []
        
        for symbol, df in self.data.items():
            summary_data.append({
                '股票代碼': symbol,
                '股票名稱': self.stock_dict.get(symbol, 'Unknown'),
                '資料筆數': len(df),
                '起始日期': df.index.min(),
                '結束日期': df.index.max(),
                '平均收盤價': df['close'].mean(),
                '最高價': df['high'].max(),
                '最低價': df['low'].min(),
            })
        
        return pd.DataFrame(summary_data)


def test_data_download():
    """測試資料下載功能"""
    print("=" * 60)
    print("測試台股資料下載功能")
    print("=" * 60)
    
    # 建立資料載入器（只測試前 3 檔股票）
    test_stocks = dict(list(SEMICONDUCTOR_STOCKS.items())[:3])
    loader = TaiwanStockDataLoader(test_stocks)
    
    # 下載最近 1 年的資料
    end_date = datetime.now().strftime('%Y-%m-%d')
    start_date = (datetime.now() - timedelta(days=365)).strftime('%Y-%m-%d')
    
    data = loader.get_processed_data(start_date, end_date)
    
    # 顯示摘要
    print("\n" + "=" * 60)
    print("資料摘要")
    print("=" * 60)
    summary = loader.get_data_summary()
    print(summary.to_string(index=False))
    
    # 顯示第一檔股票的前幾筆資料
    if data:
        first_symbol = list(data.keys())[0]
        print(f"\n{first_symbol} 前 5 筆資料:")
        print(data[first_symbol].head())
    
    print("\n✅ 測試完成！")


if __name__ == "__main__":
    test_data_download()
