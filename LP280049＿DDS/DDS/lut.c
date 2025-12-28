/*
 * lut.c
 *
 *  Created on: Dec 29, 2025
 *      Author: zerofivemlin
 */

#ifndef LUT_C_
#define LUT_C_

#include "device.h" // For type definitions like float32, uint32_t
#include <math.h>   // For sinf
#include "lut.h"    // For DDS definitions


// 硬體參數
#define SYSTEM_CLOCK_HZ     100000000.0f
#define EPWM_CLK_DIV        2.0f
#define EPWM_TIMER_PERIOD   208.0f
// Up-Down Mode: Period Count = 2 * Period = 416
#define EPWM_PERIOD_COUNTS  (2.0f * EPWM_TIMER_PERIOD)

// 理論 ISR 頻率
#define IDEAL_ISR_FREQ      (SYSTEM_CLOCK_HZ / EPWM_CLK_DIV / EPWM_PERIOD_COUNTS)

// [校正參數]
// 根據 GPIO0 (ISR/2) 的方波量測結果精確校正
// 理論值: 60.09615 kHz (120192.3 / 2)
// 量測值: 60.13700 kHz (User Measured)
// 偏差 ratio: 1.0006797
#define TARGET_TEST_FREQ    60.09615f  // kHz (Theoretical Toggle Freq)
#define MEASURED_REAL_FREQ  60.13700f  // kHz (Measured Toggle Freq)
#define CALIBRATION_RATIO   (MEASURED_REAL_FREQ / TARGET_TEST_FREQ)

// 校正後的 ISR 頻率
// 使用此真實頻率來計算 DDS Step，將可完全抵消晶振誤差
#define ISR_FREQUENCY_HZ    (IDEAL_ISR_FREQ * CALIBRATION_RATIO)
#define TWO_POW_32        4294967296.0f

// 實體化變數
#pragma DATA_SECTION(SineTable, "ramgs0"); // [關鍵] 4096點很大，要放在 GSRAM
uint16_t SineTable[DDS_TABLE_SIZE];

volatile DDS_ACC_St g_PhaseAcc = {0};
volatile uint32_t g_FreqStep = 0;
volatile uint32_t g_FreqStep_Active = 0;

// 初始化查表 (產生 0~4095 DAC 數值 - Fixed Point)
void Init_DDS_Table(void)
{
    int i;
    float rad_step = (2.0f * 3.141592654f) / (float)DDS_TABLE_SIZE;
    float rad;

    for(i = 0; i < DDS_TABLE_SIZE; i++)
    {
        rad = (float)i * rad_step;
        // 預先計算 DAC 數值：
        // sin: -1.0 ~ +1.0
        // +1.0: 0.0 ~ 2.0
        // * 2047.5: 0.0 ~ 4095.0
        // (uint16_t): 0 ~ 4095 (INT)
        SineTable[i] = (uint16_t)((sinf(rad) + 1.0f) * 2047.5f);
    }
}

// 更新頻率 (在 Main Loop 呼叫)
// 範例：Update_Frequency(60.0f); // 輸出 60Hz
void Update_Frequency(float Target_Hz)
{
    // 公式： K = F_out * (2^32 / F_s)
    // 使用 double (64-bit) 運算以獲得最高精確度，避免 float (32-bit) 尾數誤差
    double step_d = (double)Target_Hz * (4294967296.0 / (double)ISR_FREQUENCY_HZ);

    // 更新 Shadow 變數 (確保原子性)
    g_FreqStep = (uint32_t)step_d;
}

#endif /* LUT_C_ */
