/*
 * adc_control.h
 *
 * ADC Control Module Interface
 */

#ifndef ADC_MODULE_ADC_CONTROL_H_
#define ADC_MODULE_ADC_CONTROL_H_

#include <stdint.h>
#include "adc_config.h"

// ADC Data Structure
typedef struct {
    // Raw ADC Values (0-4095)
    uint16_t u16Raw_Current_A;
    uint16_t u16Raw_Current_B;
    uint16_t u16Raw_Current_C;
    uint16_t u16Raw_Voltage_DC;
    
    // Converted Values (Physical Units)
    float f32Current_A;         // Amperes
    float f32Current_B;         // Amperes
    float f32Current_C;         // Amperes
    float f32Voltage_DC;        // Volts
    
    // Filtered Values
    float f32Current_A_Filt;
    float f32Current_B_Filt;
    float f32Current_C_Filt;
    float f32Voltage_DC_Filt;
    
    // Calculated Values
    float f32Current_RMS;       // RMS current
    float f32Power;             // Power (W)
    
    // Status Flags
    uint16_t u16OverCurrent :1;
    uint16_t u16OverVoltage :1;
    uint16_t u16UnderVoltage :1;
    uint16_t u16ADC_Ready :1;
    uint16_t u16Reserved :12;
    
} ADC_Data_t;

// Global ADC Data
extern ADC_Data_t g_ADC_Data;

// Function Prototypes
extern void ADC_Module_Init(void);
extern void ADC_ReadAll(void);
extern void ADC_ProcessData(void);
extern void ADC_CheckProtection(void);
extern void task_ADC_Process(void *s);

// Inline Helper Functions
static inline float ADC_RawToCurrent(uint16_t raw)
{
    // Convert ADC value to current
    // Formula: I = (V - Voffset) / Gain
    float voltage = ((float)raw / ADC_RESOLUTION) * ADC_VREF;
    return (voltage - CURRENT_OFFSET) / CURRENT_SENSOR_GAIN;
}

static inline float ADC_RawToVoltage(uint16_t raw)
{
    // Convert ADC value to DC bus voltage
    float voltage = ((float)raw / ADC_RESOLUTION) * ADC_VREF;
    return voltage / VOLTAGE_DIVIDER_RATIO;
}

static inline float ADC_LowPassFilter(float current, float previous, float alpha)
{
    // Simple low-pass filter: y[n] = α*x[n] + (1-α)*y[n-1]
    return alpha * current + (1.0f - alpha) * previous;
}

#endif /* ADC_MODULE_ADC_CONTROL_H_ */
