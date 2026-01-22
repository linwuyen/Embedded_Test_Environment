/**
 * @file dds_core.c
 * @brief DDS 核心實作
 */
#include "dds_core.h"
#include "dds_config.h"

// 指定記憶體區段，提升效能
#pragma DATA_SECTION(LUT_Sine, DDS_SECTION_LUT_SINE); 
float LUT_Sine[DDS_LUT_SIZE];

#pragma DATA_SECTION(LUT_Arb, DDS_SECTION_LUT_ARB);
float LUT_Arb[DDS_LUT_SIZE];

#define TRI_SLOPE_CONST 0.00006103515625f 
#define PHASE_ACC_MAX_F 4294967296.0f

static inline float DDS_GetRandomFactor(uint32_t *lfsr) {
    uint32_t b = ((*lfsr >> 0) ^ (*lfsr >> 2) ^ (*lfsr >> 3) ^ (*lfsr >> 5)) & 1;
    *lfsr = (*lfsr >> 1) | (b << 15);
    return ((float)(*lfsr & 0xFFFF) * (1.0f / 32768.0f)) - 1.0f;
}

void DDS_Global_Init(void) {
    int i; const float PI = 3.14159265358979323846f;
    for(i = 0; i < DDS_LUT_SIZE; i++) {
        LUT_Sine[i] = sinf((2.0f * PI * (float)i) / (float)DDS_LUT_SIZE);
        LUT_Arb[i]  = 0.0f; 
    }
}

void DDS_InitObj(DDS_Handle_t* pDDS, float fs_hz) {
    pDDS->phase_acc = 0; pDDS->tuning_word = 0; pDDS->phase_offset = 0;
    pDDS->v_set = 0.0f; pDDS->v_offset = 0.0f; pDDS->wave_type = DDS_WAVE_SINE;
    pDDS->arb_wave_ptr = LUT_Arb;
    pDDS->harmonic_3rd_gain = 0.0f; pDDS->dither_enable = 0; pDDS->dither_amount = 0.03f; pDDS->lfsr_state = 0xACE1u;  
    pDDS->burst_enable = 0; pDDS->burst_target_cycles = 0; pDDS->burst_counter = 0; pDDS->prev_phase_acc = 0; pDDS->burst_done = 0;
    pDDS->power.mode = DDS_MODE_OPEN_LOOP; pDDS->power.v_fb = 0.0f;
    pDDS->window_start = 0; pDDS->window_end = 0xFFFFFFFF; pDDS->window_mode = 0;
    pDDS->delay_on_ticks = 0; pDDS->delay_off_ticks = 0; pDDS->dt_counter = 0; pDDS->prev_logic_state = 0;
    pDDS->sr_up_limit = 10.0f; pDDS->sr_down_limit = 10.0f; pDDS->last_output = 0.0f; pDDS->seq_inverter = 1; 
}

void DDS_SetFreq(DDS_Handle_t* pDDS, float target_freq, float fs_hz) {
    if (target_freq > DDS_MAX_FREQ) target_freq = DDS_MAX_FREQ; 
    if (target_freq < DDS_MIN_FREQ) target_freq = DDS_MIN_FREQ;
    double m = ((double)target_freq * PHASE_ACC_MAX_F) / (double)fs_hz; pDDS->tuning_word = (uint32_t)m;
}
void DDS_SetPhase(DDS_Handle_t* pDDS, float degree) {
    float target_deg = degree; if (pDDS->seq_inverter == -1 && degree != 0.0f) target_deg = 360.0f - degree;
    pDDS->phase_offset = (uint32_t)((target_deg / 360.0f) * PHASE_ACC_MAX_F);
}
void DDS_SetPhaseWindow(DDS_Handle_t* pDDS, float on_deg, float off_deg) {
    pDDS->window_start = (uint32_t)((on_deg / 360.0f) * PHASE_ACC_MAX_F);
    pDDS->window_end   = (uint32_t)((off_deg / 360.0f) * PHASE_ACC_MAX_F);
    if (pDDS->window_start <= pDDS->window_end) pDDS->window_mode = 0; else pDDS->window_mode = 1; 
}
void DDS_SetArbWaveform(DDS_Handle_t* pDDS, float* buffer) {
    if (buffer != 0) pDDS->arb_wave_ptr = buffer;
}

// [NEW] 單點更新任意波形表 (Bounds Checked)
void DDS_SetArbValue(DDS_Handle_t* pDDS, uint16_t index, float value) {
    if (index < DDS_LUT_SIZE && pDDS->arb_wave_ptr != 0) {
        // Saturation check
        if (value > 1.0f) value = 1.0f;
        if (value < -1.0f) value = -1.0f;
        pDDS->arb_wave_ptr[index] = value;
    }
}

// [NEW] 批量套用參數 (Decoupled from IPC)
void DDS_ApplySettings(DDS_Handle_t* pDDS, const DDS_Transfer_t* pData) {
    pDDS->tuning_word = pData->tuning_word;
    pDDS->v_set       = pData->v_set;
    pDDS->v_offset    = pData->v_offset;
    pDDS->wave_type   = (DDS_WaveType)pData->wave_type;
    
    // Apply Feature Flags
    if (pData->feature_flags & DDS_FLAG_DITHER_EN) {
        pDDS->dither_enable = 1;
        pDDS->dither_amount = pData->dither_amount;
    } else {
        pDDS->dither_enable = 0;
    }

    if (pData->feature_flags & DDS_FLAG_3RD_HARM_EN) {
        pDDS->harmonic_3rd_gain = pData->harmonic_3rd_gain;
    } else {
        pDDS->harmonic_3rd_gain = 0.0f;
    }
    
    // Deadtime
    if (pData->feature_flags & DDS_FLAG_DEADTIME_EN) {
        pDDS->delay_on_ticks  = pData->deadtime_ticks;
        pDDS->delay_off_ticks = pData->deadtime_ticks;
    } else {
        pDDS->delay_on_ticks  = 0;
        pDDS->delay_off_ticks = 0;
    }

    // Slew Rate
    if (pData->feature_flags & DDS_FLAG_SLEW_LIMIT_EN) {
        pDDS->sr_up_limit   = pData->slew_rate;
        pDDS->sr_down_limit = pData->slew_rate;
    } else {
        // Disable limit by setting to very high value
        pDDS->sr_up_limit   = 1000.0f; 
        pDDS->sr_down_limit = 1000.0f;
    }

    // Phase Cutting (Dimmer)
    if (pData->feature_flags & DDS_FLAG_PHASE_CUT_EN) {
        DDS_SetPhaseWindow(pDDS, (float)pData->phase_cut_on, (float)pData->phase_cut_off);
    } else {
        // Disable windowing (Full Wave)
        // DDS_SetPhaseWindow logic: if start <= end, mode=0 (Active In Range)
        // 0 to 360 covers everything. Or logic: if start > end, mode=1.
        // Let's use 0 to 360 which is safe.
        // Actually SetPhaseWindow(0, 0) might be interpreted as tiny window?
        // Let's use internal knowledge: window_end needs to be MAX.
        DDS_SetPhaseWindow(pDDS, 0.0f, 360.0f);
    }
}

#pragma CODE_SECTION(DDS_Update, ".TI.ramfunc");
float DDS_Update(DDS_Handle_t* pDDS) {
    // 檢查硬體保護
    DDS_Hardware_Protect_Check(pDDS);

    if (pDDS->burst_enable && pDDS->burst_done) { pDDS->last_output = 0.0f; return 0.0f; }
    
    uint32_t step = pDDS->tuning_word;
    if (pDDS->dither_enable) {
        float rand = DDS_GetRandomFactor(&pDDS->lfsr_state);
        step += (int32_t)((float)step * pDDS->dither_amount * rand);
    }
    pDDS->phase_acc += step;
    
    if (pDDS->phase_acc < pDDS->prev_phase_acc) {
        if (pDDS->burst_enable) {
            pDDS->burst_counter++; if (pDDS->burst_counter >= pDDS->burst_target_cycles) pDDS->burst_done = 1; 
        }
    }
    pDDS->prev_phase_acc = pDDS->phase_acc;

    uint32_t curr_phase = pDDS->phase_acc + pDDS->phase_offset;
    float raw_val = 0.0f; uint16_t is_active = 0;
    
    if (pDDS->window_mode == 0) { if (curr_phase >= pDDS->window_start && curr_phase <= pDDS->window_end) is_active = 1; } 
    else { if (curr_phase >= pDDS->window_start || curr_phase <= pDDS->window_end) is_active = 1; }
    
    if (is_active) {
        if (pDDS->wave_type == DDS_WAVE_SINE) {
            uint16_t idx = curr_phase >> 22; raw_val = LUT_Sine[idx];
            if (pDDS->harmonic_3rd_gain > 0.001f) {
                uint32_t p3 = curr_phase * 3; uint16_t i3 = p3 >> 22; raw_val += LUT_Sine[i3] * pDDS->harmonic_3rd_gain;
            }
        } else if (pDDS->wave_type == DDS_WAVE_SQUARE) {
            if ((int32_t)curr_phase >= 0) raw_val = 1.0f; else raw_val = -1.0f;
        } else if (pDDS->wave_type == DDS_WAVE_TRIANGLE) {
            uint16_t p16 = curr_phase >> 16;
            if (p16 < 32768) raw_val = ((float)p16 * TRI_SLOPE_CONST) - 1.0f; else raw_val = 1.0f - ((float)(p16 - 32768) * TRI_SLOPE_CONST);
        } else if (pDDS->wave_type == DDS_WAVE_ARBITRARY) {
            uint16_t idx = curr_phase >> 22; raw_val = pDDS->arb_wave_ptr[idx];
        }
    } else { raw_val = 0.0f; }

    uint16_t logic = (raw_val >= 0.0f) ? 1 : 0;
    if (logic != pDDS->prev_logic_state) {
        if (logic == 1) pDDS->dt_counter = pDDS->delay_on_ticks; else pDDS->dt_counter = pDDS->delay_off_ticks;
    }
    pDDS->prev_logic_state = logic;
    if (pDDS->dt_counter > 0) { pDDS->dt_counter--; raw_val = 0.0f; }
    
    float target = (pDDS->power.mode == DDS_MODE_OPEN_LOOP) ? (raw_val * pDDS->v_set) + pDDS->v_offset : (raw_val * pDDS->v_set); 
    float delta = target - pDDS->last_output;
    if (delta > pDDS->sr_up_limit) delta = pDDS->sr_up_limit; else if (delta < -pDDS->sr_down_limit) delta = -pDDS->sr_down_limit;
    pDDS->last_output += delta;
    
    if (pDDS->burst_enable && pDDS->burst_done) { if (fabsf(pDDS->last_output) < 0.01f) return 0.0f; }
    return pDDS->last_output;
}