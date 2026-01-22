/**
 * @file dds_core.h
 * @brief DDS 核心模組介面
 */
#ifndef DDS_CORE_H
#define DDS_CORE_H
#include "dds_defs.h"
// ...
void DDS_SetArbWaveform(DDS_Handle_t* pDDS, float* buffer);

// [NEW] Batch Update API (Decoupled)
void DDS_ApplySettings(DDS_Handle_t* pDDS, const DDS_Transfer_t* pData);

float DDS_Update(DDS_Handle_t* pDDS);

#endif

#define DDS_LUT_SIZE 1024 

typedef enum {
    DDS_WAVE_SINE = 0,
    DDS_WAVE_SQUARE,
    DDS_WAVE_TRIANGLE,
    DDS_WAVE_ARBITRARY 
} DDS_WaveType;

typedef enum { DDS_MODE_OPEN_LOOP = 0, DDS_MODE_VOLTAGE_CLOSE } DDS_CtrlMode;

typedef struct {
    uint32_t phase_acc; uint32_t tuning_word; uint32_t phase_offset;   
    float v_set; float v_offset; 
    DDS_WaveType wave_type;  
    float* arb_wave_ptr; 
    float harmonic_3rd_gain; uint16_t dither_enable; float dither_amount; uint32_t lfsr_state;        
    uint16_t burst_enable; uint32_t burst_target_cycles; uint32_t burst_counter; 
    uint32_t prev_phase_acc; uint16_t burst_done;
    struct { DDS_CtrlMode mode; float v_fb; float v_bus; float kp; float ki; float err_integ; } power;
    uint32_t window_start; uint32_t window_end; uint16_t window_mode;    
    uint16_t delay_on_ticks; uint16_t delay_off_ticks; uint16_t dt_counter; uint16_t prev_logic_state;
    float sr_up_limit; float sr_down_limit; float last_output; int16_t seq_inverter;   
} DDS_Handle_t;

void DDS_Global_Init(void);
void DDS_InitObj(DDS_Handle_t* pDDS, float fs_hz);
void DDS_SetFreq(DDS_Handle_t* pDDS, float target_freq, float fs_hz);
void DDS_SetPhase(DDS_Handle_t* pDDS, float degree);
void DDS_SetPhaseWindow(DDS_Handle_t* pDDS, float on_deg, float off_deg);
void DDS_SetArbWaveform(DDS_Handle_t* pDDS, float* buffer);
float DDS_Update(DDS_Handle_t* pDDS);

#endif