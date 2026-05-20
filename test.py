import os
from google import genai
from google.genai import types

    # 不要在程式碼中硬編碼 API 金鑰。請在系統環境或 CI/Secrets 中設定
    # 例如在 PowerShell 中：
    # $env:GEMINI_API_KEY = "<YOUR_API_KEY>"
    # 以下保留為示意（已註解），實際使用請改為環境變數。
    # os.environ["GEMINI_API_KEY"] = "<YOUR_API_KEY>"

    # 實例化客戶端
try:
        client = genai.Client()
except Exception as e:
        print(f"初始化失敗: {e}")
        exit()

def test_connection():
        print("====== 正在向 Google 雲端叢集發起推論請求 ======")
        
        response = client.models.generate_content(
            model='gemini-2.5-flash-lite',
            contents='請用一句話指出 TI C28x 架構中，哪個硬體模組最常被用來觸發 ePWM 的中斷？',
            config=types.GenerateContentConfig(
                temperature=0.0, 
            ),
        )
        
        print("====== 雲端推論結果 ======")
        print(response.text)

if __name__ == "__main__":
        test_connection()
