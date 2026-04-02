/*
 * adc_control.c
 *
 * ADC Control Module Implementation
 */

#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "adc_control.h"
#include <math.h>

// Global ADC Data Instance
ADC_Data_t g_ADC_Data = {0};

// Private Variables
static uint32_t s_u32SampleCount = 0;

/*
 * ADC_Module_Init
 * Initialize ADC module (called from main)
 */
void ADC_Module_Init(void)
{
    // Initialize data structure
    g_ADC_Data.u16Raw_Current_A = 0;
    g_ADC_Data.u16Raw_Current_B = 0;
    g_ADC_Data.u16Raw_Current_C = 0;
    g_ADC_Data.u16Raw_Voltage_DC = 0;
    
    g_ADC_Data.f32Current_A = 0.0f;
    g_ADC_Data.f32Current_B = 0.0f;
    g_ADC_Data.f32Current_C = 0.0f;
    g_ADC_Data.f32Voltage_DC = 0.0f;
    
    g_ADC_Data.f32Current_A_Filt = 0.0f;
    g_ADC_Data.f32Current_B_Filt = 0.0f;
    g_ADC_Data.f32Current_C_Filt = 0.0f;
    g_ADC_Data.f32Voltage_DC_Filt = 0.0f;
    
    g_ADC_Data.u16OverCurrent = 0;
    g_ADC_Data.u16OverVoltage = 0;
    g_ADC_Data.u16UnderVoltage = 0;
    g_ADC_Data.u16ADC_Ready = 1;
    
    s_u32SampleCount = 0;
    
    // Note: ADC hardware initialization is done in Board_init() via SysConfig
}

/*
 * ADC_ReadAll
 * Read all ADC channels (called from ISR)
 */
void ADC_ReadAll(void)
{
    // Read raw ADC values (using configured ADC from SysConfig)
    g_ADC_Data.u16Raw_Current_A = ADC_readResult(CV_AD_RESULT_BASE, CV_AD_SOC0);
    g_ADC_Data.u16Raw_Current_B = 0;  // Not configured in SysConfig
    g_ADC_Data.u16Raw_Current_C = 0;  // Not configured in SysConfig
    g_ADC_Data.u16Raw_Voltage_DC = 0; // Not configured in SysConfig
    
    s_u32SampleCount++;
}

/*
 * ADC_ProcessData
 * Convert and filter ADC data (called from background task)
 */
void ADC_ProcessData(void)
{
    // Convert raw values to physical units
    g_ADC_Data.f32Current_A = ADC_RawToCurrent(g_ADC_Data.u16Raw_Current_A);
    g_ADC_Data.f32Current_B = ADC_RawToCurrent(g_ADC_Data.u16Raw_Current_B);
    g_ADC_Data.f32Current_C = ADC_RawToCurrent(g_ADC_Data.u16Raw_Current_C);
    g_ADC_Data.f32Voltage_DC = ADC_RawToVoltage(g_ADC_Data.u16Raw_Voltage_DC);
    
    // Apply low-pass filter
    g_ADC_Data.f32Current_A_Filt = ADC_LowPassFilter(
        g_ADC_Data.f32Current_A,
        g_ADC_Data.f32Current_A_Filt,
        ADC_FILTER_ALPHA
    );
    
    g_ADC_Data.f32Current_B_Filt = ADC_LowPassFilter(
        g_ADC_Data.f32Current_B,
        g_ADC_Data.f32Current_B_Filt,
        ADC_FILTER_ALPHA
    );
    
    g_ADC_Data.f32Current_C_Filt = ADC_LowPassFilter(
        g_ADC_Data.f32Current_C,
        g_ADC_Data.f32Current_C_Filt,
        ADC_FILTER_ALPHA
    );
    
    g_ADC_Data.f32Voltage_DC_Filt = ADC_LowPassFilter(
        g_ADC_Data.f32Voltage_DC,
        g_ADC_Data.f32Voltage_DC_Filt,
        ADC_FILTER_ALPHA
    );
    
    // Calculate RMS current (simplified: use max of three phases)
    float maxCurrent = g_ADC_Data.f32Current_A_Filt;
    if (g_ADC_Data.f32Current_B_Filt > maxCurrent) 
        maxCurrent = g_ADC_Data.f32Current_B_Filt;
    if (g_ADC_Data.f32Current_C_Filt > maxCurrent) 
        maxCurrent = g_ADC_Data.f32Current_C_Filt;
    
    g_ADC_Data.f32Current_RMS = maxCurrent;
    
    // Calculate power (simplified: P = V * I)
    g_ADC_Data.f32Power = g_ADC_Data.f32Voltage_DC_Filt * g_ADC_Data.f32Current_RMS;
}

/*
 * ADC_CheckProtection
 * Check protection thresholds (called from ISR or background task)
 */
void ADC_CheckProtection(void)
{
    // Over-current check
    if (g_ADC_Data.f32Current_RMS > OVERCURRENT_THRESHOLD)
    {
        g_ADC_Data.u16OverCurrent = 1;
        // TODO: Trigger protection action (disable SPWM, etc.)
    }
    else
    {
        g_ADC_Data.u16OverCurrent = 0;
    }
    
    // Over-voltage check
    if (g_ADC_Data.f32Voltage_DC > OVERVOLTAGE_THRESHOLD)
    {
        g_ADC_Data.u16OverVoltage = 1;
        // TODO: Trigger protection action
    }
    else
    {
        g_ADC_Data.u16OverVoltage = 0;
    }
    
    // Under-voltage check
    if (g_ADC_Data.f32Voltage_DC < UNDERVOLTAGE_THRESHOLD)
    {
        g_ADC_Data.u16UnderVoltage = 1;
        // TODO: Trigger protection action
    }
    else
    {
        g_ADC_Data.u16UnderVoltage = 0;
    }
}

/*
 * task_ADC_Process
 * Time task for ADC processing (called from time task system)
 */
void task_ADC_Process(void *s)
{
    // Process ADC data
    ADC_ProcessData();
    
    // Check protection
    ADC_CheckProtection();
    
    // Update status
    g_ADC_Data.u16ADC_Ready = 1;
}
