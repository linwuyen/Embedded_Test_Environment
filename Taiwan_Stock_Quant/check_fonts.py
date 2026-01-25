import matplotlib.pyplot as plt
import matplotlib
import os
import shutil

def test_plot():
    print(f"Matplotlib cache dir: {matplotlib.get_cachedir()}")
    
    # Force the font
    plt.rcParams['font.sans-serif'] = ['Microsoft JhengHei']
    plt.rcParams['axes.unicode_minus'] = False
    
    fig, ax = plt.subplots()
    ax.set_title('測試 Title')
    ax.text(0.5, 0.5, '測試 Text', ha='center')
    
    try:
        plt.savefig('test_font_render.png')
        print("Saved test_font_render.png")
    except Exception as e:
        print(f"Save failed: {e}")

if __name__ == "__main__":
    test_plot()
