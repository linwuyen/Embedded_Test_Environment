/*
 * WaveGen_Manager.c
 *
 * Waveform Generator Manager Implementation
 */

#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "WaveGen_Manager.h"
#include "../DDS_Module/DDS.h"
#include "../SPWM_Ctrl.h"

// Global Status
WaveGen_Status_t g_WaveGen_Status = {0};

/*
 * WaveGen_Init
 * Initialize waveform generator manager
 */
void WaveGen_Init(void)
{
    // Initialize status
    g_WaveGen_Status.eMode = WAVEGEN_MODE_SPWM;  // Default to SPWM
    g_WaveGen_Status.u16Enabled = 0;
    g_WaveGen_Status.u16ModeChanged = 0;
    
    g_WaveGen_Status.f32Frequency_Hz = 60.0f;
    g_WaveGen_Status.f32Amplitude_V = 1.65f;
    g_WaveGen_Status.f32Offset_V = 1.65f;
    g_WaveGen_Status.f32Phase_Deg = 0.0f;
    
    // Initialize both generators
    DDS_Init();
    // SPWM_Init(); // If SPWM has init function
}

/*
 * WaveGen_SetMode
 * Switch between SPWM and DDS modes
 */
void WaveGen_SetMode(WaveGen_Mode_t mode)
{
    if (g_WaveGen_Status.eMode == mode) {
        return;  // Already in this mode
    }
    
    // Disable current generator before switching
    if (g_WaveGen_Status.u16Enabled) {
        WaveGen_Enable(0);
    }
    
    // Switch mode
    g_WaveGen_Status.eMode = mode;
    g_WaveGen_Status.u16ModeChanged = 1;
    
    // Apply current parameters to new mode
    WaveGen_SetFrequency(g_WaveGen_Status.f32Frequency_Hz);
    WaveGen_SetAmplitude(g_WaveGen_Status.f32Amplitude_V);
    WaveGen_SetOffset(g_WaveGen_Status.f32Offset_V);
    WaveGen_SetPhase(g_WaveGen_Status.f32Phase_Deg);
}

/*
 * WaveGen_SetFrequency
 * Set output frequency (unified interface)
 */
void WaveGen_SetFrequency(float freq_hz)
{
    g_WaveGen_Status.f32Frequency_Hz = freq_hz;
    
    if (g_WaveGen_Status.eMode == WAVEGEN_MODE_DDS) {
        // DDS Mode
        DDS.Cmd.fFreq_Hz = freq_hz;
    } else {
        // SPWM Mode
        // SPWM_SetFrequency(freq_hz); // If SPWM has this function
    }
}

/*
 * WaveGen_SetAmplitude
 * Set output amplitude (unified interface)
 */
void WaveGen_SetAmplitude(float amp_v)
{
    g_WaveGen_Status.f32Amplitude_V = amp_v;
    
    if (g_WaveGen_Status.eMode == WAVEGEN_MODE_DDS) {
        // DDS Mode
        DDS.Cmd.fAmp_V = amp_v;
    } else {
        // SPWM Mode
        // SPWM_SetAmplitude(amp_v); // If SPWM has this function
    }
}

/*
 * WaveGen_SetOffset
 * Set DC offset (unified interface)
 */
void WaveGen_SetOffset(float offset_v)
{
    g_WaveGen_Status.f32Offset_V = offset_v;
    
    if (g_WaveGen_Status.eMode == WAVEGEN_MODE_DDS) {
        // DDS Mode
        DDS.Cmd.fOffset_V = offset_v;
    } else {
        // SPWM Mode
        // SPWM_SetOffset(offset_v); // If SPWM has this function
    }
}

/*
 * WaveGen_SetPhase
 * Set phase offset (unified interface)
 */
void WaveGen_SetPhase(float phase_deg)
{
    g_WaveGen_Status.f32Phase_Deg = phase_deg;
    
    if (g_WaveGen_Status.eMode == WAVEGEN_MODE_DDS) {
        // DDS Mode
        DDS.Cmd.fPhase_Deg = phase_deg;
    } else {
        // SPWM Mode
        // SPWM_SetPhase(phase_deg); // If SPWM has this function
    }
}

/*
 * WaveGen_Enable
 * Enable/Disable waveform generator (unified interface)
 */
void WaveGen_Enable(uint16_t enable)
{
    g_WaveGen_Status.u16Enabled = enable ? 1 : 0;
    
    if (g_WaveGen_Status.eMode == WAVEGEN_MODE_DDS) {
        // DDS Mode
        DDS.Cmd.u16CmdEnable = enable;
    } else {
        // SPWM Mode
        // SPWM_Enable(enable); // If SPWM has this function
    }
}

/*
 * WaveGen_Task
 * Background task for waveform generator (called from time task)
 */
void WaveGen_Task(void)
{
    if (g_WaveGen_Status.eMode == WAVEGEN_MODE_DDS) {
        // DDS Mode - call DDS background task
        DDS_Task();
    } else {
        // SPWM Mode - call SPWM background task if exists
        // SPWM_Task();
    }
    
    // Clear mode changed flag
    g_WaveGen_Status.u16ModeChanged = 0;
}

/*
 * WaveGen_ISR_Update
 * ISR update function (called from EPWM ISR)
 * Routes to appropriate generator based on mode
 */
void WaveGen_ISR_Update(void)
{
    if (g_WaveGen_Status.eMode == WAVEGEN_MODE_DDS) {
        // DDS Mode - run DDS core
        uint16_t dacValue = DDS_Run_Core(&DDS);
        
        // Output to DAC or PWM
        // Option 1: If DAC available
        // DAC_setShadowValue(myDAC0_BASE, dacValue);
        
        // Option 2: Use PWM to simulate DAC
        float duty = (float)dacValue / 4095.0f;
        uint16_t period = EPWM_getTimeBasePeriod(myEPWM0_BASE);
        EPWM_setCounterCompareValue(myEPWM0_BASE, 
                                    EPWM_COUNTER_COMPARE_A, 
                                    (uint16_t)(duty * period));
    } else {
        // SPWM Mode - use original SPWM output
        // SPWM_ISR_Update(); // If SPWM has ISR function
    }
}
