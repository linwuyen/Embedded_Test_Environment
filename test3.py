import os
from google import genai
from google.genai import types

# 請勿在程式碼中硬編碼金鑰。於執行環境設定 `GEMINI_API_KEY`。
# 例如 PowerShell:
# $env:GEMINI_API_KEY = "<YOUR_API_KEY>"

# 1. 定義你要掃描的副檔名 (C2000 開發核心檔案)
TARGET_EXTENSIONS = ['.c', '.h', '.cmd', '.asm']

def read_project_files(project_path):
    print(f"====== 開始掃描專案目錄: {project_path} ======")
    project_content = ""
    file_count = 0
    
    # 2. 遍歷資料夾
    for root, _, files in os.walk(project_path):
        for file in files:
            if any(file.endswith(ext) for ext in TARGET_EXTENSIONS):
                file_path = os.path.join(root, file)
                try:
                    # C2000 原始碼有時會包含特殊字元，使用 errors='ignore' 避免中斷
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                        # 3. 加上標籤，讓模型知道這是哪個檔案
                        project_content += f"\n\n{'='*20}\n[檔案路徑]: {file_path}\n{'='*20}\n"
                        project_content += content
                        file_count += 1
                except Exception as e:
                    print(f"無法讀取檔案 {file_path}: {e}")
                    
    print(f"掃描完成。共讀取了 {file_count} 個檔案。")
    return project_content

def c2000_project_agent(project_path):
    # 讀取整個專案內容
    full_project_text = read_project_files(project_path)
    
    if not full_project_text:
        print("未找到任何符合的程式碼檔案。")
        return

    try:
        client = genai.Client()
    except Exception as e:
        print(f"API 初始化失敗: {e}")
        return

    expert_config = types.GenerateContentConfig(
        temperature=0.0,
        system_instruction=(
            "你現在是全球 Top 0.1% 的 TI C28x 與數位電源控制韌體專家。"
            "請跳過所有基礎解釋與常識普及，不提供任何情緒價值。"
            "直接使用「第一性原理」進行深度邏輯與暫存器級別推導。"
            "若涉及效能瓶頸、中斷延遲或系統趨勢判斷，必須附帶具體數值、區間或機率（%）。"
        )
    )
    
    print("\n====== 將專案架構載入模型記憶體 (這可能需要幾秒鐘) ======")
    chat = client.chats.create(
        model='gemini-2.5-flash',
        config=expert_config
    )
    
    # 4. 將整個專案內容作為「初始背景」發送給模型
    initial_prompt = f"以下是我目前的 C28x 韌體專案架構與完整程式碼。\n請閱讀並記憶這些架構，接下來我將針對這個專案向你提問。\n\n{full_project_text}"
    
    try:
        response = chat.send_message(initial_prompt)
        print("\n[專案載入完成]:")
        print(response.text)
    except Exception as e:
        print(f"\n[載入失敗，可能超出 Token 上限或網路連線問題]: {e}")
        return

    print("\n系統就緒。請開始針對專案提問。(輸入 'exit' 關閉引擎)\n")
    print("-" * 50)
    
    while True:
        user_input = input(">> [韌體工程師]: ")
        if user_input.lower() in ['exit', 'quit']:
            break
        if not user_input.strip():
            continue
            
        print("<< [代碼庫檢索與解析中...]")
        try:
            response = chat.send_message(user_input)
            print(f"\n[除錯診斷]:\n{response.text}\n")
            print("-" * 50)
        except Exception as e:
            print(f"\n[推論異常]: {e}\n")

if __name__ == "__main__":
    # 將這裡替換為你實際的 C2000 專案資料夾路徑 (請使用絕對路徑)
    # Windows 路徑範例: r"C:\ti\c2000\C2000Ware_X_XX_XX_XX\driverlib\f28004x\examples"
    TARGET_PROJECT_DIR = r"D:\YOUR_PROJECT_PATH" 
    
    c2000_project_agent(TARGET_PROJECT_DIR)