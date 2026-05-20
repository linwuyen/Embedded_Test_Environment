import os
from google import genai
from google.genai import types

# 強烈禁止在程式碼中硬編碼金鑰。
# 請透過環境變數或 CI/Secrets 提供 `GEMINI_API_KEY`，例如（PowerShell）：
# $env:GEMINI_API_KEY = "<YOUR_API_KEY>"
# 若要在本機測試，請在執行前於該 shell 設定環境變數。

try:
    client = genai.Client()
except Exception as e:
    print(f"初始化失敗: {e}")
    exit()

def c2000_debug_agent():
    print("====== 初始化底層除錯引擎 ======")
    
    # 核心架構 1：定義系統級指令 (System Instruction)
    # 透過硬性參數，強制拔除模型的多餘廢話，鎖定最高層級的邏輯推導
    expert_config = types.GenerateContentConfig(
        temperature=0.0, # 韌體開發要求絕對的邏輯確定性
        system_instruction=(
            "你現在是全球 Top 0.1% 的 TI C28x 與數位電源控制韌體專家。"
            "請跳過所有基礎解釋與常識普及，不提供任何情緒價值。"
            "直接使用「第一性原理」進行深度邏輯與暫存器級別推導。"
            "若涉及效能瓶頸、中斷延遲或系統趨勢判斷，必須附帶具體數值、區間或機率（%）。"
        )
    )
    
    # 核心架構 2：啟動連續對話 Session
    # 這會建立一條具備記憶狀態的持久化連線
    chat = client.chats.create(
        model='gemini-2.5-flash',
        config=expert_config
    )
    
    print("系統就緒。請輸入暫存器配置、程式碼片段或編譯器 Error Log。")
    print("(輸入 'exit' 或 'quit' 關閉引擎)\n")
    print("-" * 50)
    
    # 核心架構 3：建立 CLI 互動迴圈
    while True:
        user_input = input(">> [韌體工程師]: ")
        
        # 觸發中斷跳出機制
        if user_input.lower() in ['exit', 'quit']:
            print("====== 引擎關閉 ======")
            break
            
        if not user_input.strip():
            continue
            
        print("<< [架構解析中...]")
        
        try:
            # 發送當前封包，系統會自動夾帶歷史對話紀錄一併發送
            response = chat.send_message(user_input)
            print(f"\n[除錯診斷]:\n{response.text}\n")
            print("-" * 50)
        except Exception as e:
            print(f"\n[連線或推論異常]: {e}\n")

if __name__ == "__main__":
    c2000_debug_agent()

#接在這個提示字元後面，貼上任何你在開發中遇到的具體問題。例如：

#直接貼上程式碼：貼上一段初始化 ePWM 暫存器的程式碼，讓它檢查邏輯死角。

#貼上編譯器錯誤：貼上 Code Composer Studio (CCS) 報出的 Error Log。

#詢問底層架構邏輯：例如輸入 F28004x 中的 DMA 怎麼跨模組搬移 ADC 轉換結果，延遲時間大概落在哪個區間？