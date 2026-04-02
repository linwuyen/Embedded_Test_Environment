/*
 * WaveGen_Manager.h
 *
 * Waveform Generator Manager
 * Manages SPWM and DDS mode switching
 */

#ifndef WAVEGEN_MANAGER_WAVEGEN_MANAGER_H_
#define WAVEGEN_MANAGER_WAVEGEN_MANAGER_H_

#include <stdint.h>

// Waveform Generator Modes
typedef enum {
    WAVEGEN_MODE_SPWM = 0,  // SPWM Mode (Original)
    WAVEGEN_MODE_DDS  = 1   // DDS Mode (High Precision)
} WaveGen_Mode_t;

// Waveform Generator Status
typedef struct {
    WaveGen_Mode_t eMode;           // Current mode
    uint16_t u16Enabled :1;         // Generator enabled
    uint16_t u16ModeChanged :1;     // Mode changed flag
    uint16_t u16Reserved :14;
    
    // Common Parameters
    float f32Frequency_Hz;          // Output frequency (Hz)
    float f32Amplitude_V;           // Amplitude (V)
    float f32Offset_V;              // DC offset (V)
    float f32Phase_Deg;             // Phase (degrees)
    
} WaveGen_Status_t;

// Global Status
extern WaveGen_Status_t g_WaveGen_Status;

// Function Prototypes
extern void WaveGen_Init(void);
extern void WaveGen_SetMode(WaveGen_Mode_t mode);
extern void WaveGen_SetFrequency(float freq_hz);
extern void WaveGen_SetAmplitude(float amp_v);
extern void WaveGen_SetOffset(float offset_v);
extern void WaveGen_SetPhase(float phase_deg);
extern void WaveGen_Enable(uint16_t enable);
extern void WaveGen_Task(void);

// ISR Helper (called from EPWM ISR)
extern void WaveGen_ISR_Update(void);

#endif /* WAVEGEN_MANAGER_WAVEGEN_MANAGER_H_ */
