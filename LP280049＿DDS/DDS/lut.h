/*
 * lut.h
 *
 *  Created on: Dec 29, 2025
 *      Author: zerofivemlin
 */

#ifndef LUT_H_
#define LUT_H_

#include <stdint.h>

// 1. 定義查表大小
#define DDS_TABLE_SIZE  4096   // 2^12
#define DDS_SHIFT_BITS  20     // 32 - 12 = 20 (右移位數)

// 2. 定義相位累加器結構 (Union)
// 注意：TI C28x Compiler 預設為 LSB First (低位在前)
typedef union {
    uint32_t u32Accumulator; // 32-bit 完整數值 (用來做加法)
    struct {
        uint32_t u32Decimal : 20; // 低 20 位 (小數部分)
        uint32_t u32Index   : 12; // 高 12 位 (整數部分 -> 對應 0~4095)
    } bit;
} DDS_ACC_St;

// 3. 全域變數宣告
extern uint16_t SineTable[DDS_TABLE_SIZE]; // 宣告表 (Fixed Point: DAC Values 0-4095)
extern volatile DDS_ACC_St g_PhaseAcc;    // 相位累加器
extern volatile uint32_t g_FreqStep;      // 步進值 (Shadow)
extern volatile uint32_t g_FreqStep_Active; // 步進值 (ISR用)

// 4. 初始化函式原型
void Init_DDS_Table(void);
void Update_Frequency(float Target_Hz);



#endif /* LUT_H_ */
