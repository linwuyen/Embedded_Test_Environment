/**
 * @file dds_service.c
 * @brief DDS 應用邏輯 (含 Soft Start/Stop 與 Phase Reset)
 */
#include "dds_service.h"
// [Removed] #include "dds_ipc.h" -> Service Layer is now pure.
#include "dds_config.h"

volatile Modbus_Map_t g_ModbusData;
static DDS_Handle_t dds; 
static volatile uint16_t g_Task_1ms_Flag = 0; 

// Ramping State
// [NEW] S-Curve Ramping Structure
typedef struct {
    float start;
    float current;
    float target;
    uint32_t t_elapsed_ms;
    uint32_t t_duration_ms;
} RampState_t;

static RampState_t g_Ramp_Freq;
static RampState_t g_Ramp_Vset;
static RampState_t g_Ramp_Voffset;
static RampState_t g_Ramp_Harm3;

// 啟動控制狀態
static float g_Last_Freq_Cmd = -1.0f; 
static float g_Last_Vset_Cmd = -1.0f; 
static float g_Last_Voffset_Cmd = -100.0f; 
static float g_Last_Harm3_Cmd = -1.0f;

// [NEW] S-Curve Calculation (SmoothStep: 3t^2 - 2t^3)
static float Calculate_SCurve(float start, float end, float t_ratio) {
    if (t_ratio <= 0.0f) return start;
    if (t_ratio >= 1.0f) return end;
    
    // SmoothStep
    float factor = t_ratio * t_ratio * (3.0f - 2.0f * t_ratio);
    return start + (end - start) * factor;
}

#define SEQ_MAX_STEPS 4
static Seq_Instruction_t g_Sequence[SEQ_MAX_STEPS]; // [Update] No longer const/hardcoded

volatile uint16_t g_Seq_Run = 0; 
static uint16_t g_Seq_Index = 0;
// ...

// ...

// FSM State
static DDS_State_t g_CurrentState = DDS_STATE_IDLE;

static void DDS_Service_UpdateFSM(void) {
    // 1. Check Transitions
    DDS_State_t next_state = g_CurrentState;
    
    // Check Hardware Fault (Stub)
    // if (Hardware_Fault) next_state = DDS_STATE_FAULT;
    
    switch (g_CurrentState) {
        case DDS_STATE_IDLE:
            if (g_ModbusData.reg_Enable) {
                if (g_ModbusData.reg_SeqRun) {
                    next_state = DDS_STATE_RUN_SEQUENCE;
                } else {
                    next_state = DDS_STATE_RUN_MANUAL;
                }
            }
            break;
            
        case DDS_STATE_RUN_MANUAL:
            if (!g_ModbusData.reg_Enable) {
                next_state = DDS_STATE_IDLE;
            } else if (g_ModbusData.reg_SeqRun) {
                next_state = DDS_STATE_RUN_SEQUENCE;
            }
            break;
            
        case DDS_STATE_RUN_SEQUENCE:
            if (!g_ModbusData.reg_Enable) {
                next_state = DDS_STATE_IDLE;
            } else if (!g_ModbusData.reg_SeqRun) {
                next_state = DDS_STATE_RUN_MANUAL;
            } else if (g_Seq_Run == 0 && g_Seq_Index == 0) { 
                // Checks if sequence finished (g_Seq_Run logic handled inside tick)
                // Actually, g_Seq_Run is the run flag. 
                // If sequence finishes, we might want to go to MANUAL or IDLE?
                // For now, let's allow user to switch back manually or if logic auto-clears reg_SeqRun.
            }
            break;
            
        case DDS_STATE_FAULT:
            if (!g_ModbusData.reg_Enable) {
                 // Reset condition: User must toggle Enable OFF
                 next_state = DDS_STATE_IDLE;
                 g_ModbusData.reg_FaultCode = 0;
            }
            break;
    }
    
    // 2. On Exit Actions (Transitions)
    if (next_state != g_CurrentState) {
        if (g_CurrentState == DDS_STATE_IDLE && next_state != DDS_STATE_IDLE) {
            // Start-up: Reset IO Params
            dds.phase_acc = 0;
            // Reset Ramps
            g_Ramp_Freq.current = g_ModbusData.reg_Freq.f32; g_Ramp_Freq.target = g_Ramp_Freq.current;
            g_Ramp_Vset.current = 0.0f; g_Ramp_Vset.target = 0.0f;
            g_Ramp_Voffset.current = 0.0f; g_Ramp_Voffset.target = 0.0f;
            g_Ramp_Harm3.current = 0.0f; g_Ramp_Harm3.target = 0.0f;
            
            g_Last_Freq_Cmd = g_Ramp_Freq.current; 
            g_Last_Vset_Cmd = 0.0f; 
            g_Last_Voffset_Cmd = 0.0f;
            g_Last_Harm3_Cmd = 0.0f;
        }
        g_CurrentState = next_state;
        g_ModbusData.reg_CurrentState = (uint16_t)g_CurrentState;
    }

    // 3. State Actions
    
    // -- Safety Override --
    if (g_CurrentState == DDS_STATE_IDLE || g_CurrentState == DDS_STATE_FAULT) {
        g_Ramp_Vset.target = 0.0f; g_Ramp_Voffset.target = 0.0f; g_Ramp_Harm3.target = 0.0f;
        // Instant Zero for safety
        g_Ramp_Vset.current = 0.0f; g_Ramp_Voffset.current = 0.0f; g_Ramp_Harm3.current = 0.0f;
        
    } else {
        // -- RUNNING Modes --
        
        float cmd_freq, cmd_vset, cmd_voff, cmd_harm3;
        
        // Source Selection
        if (g_CurrentState == DDS_STATE_RUN_SEQUENCE) {
            // Sequence Logic updates Modbus Regs (in Tick), so we just read Modbus Regs?
            // Wait, previous logic had "if (g_Seq_Run == 0) { Manual Logic }".
            // Now, Sequence Logic (in Tick) should update `g_ModbusData` or we read discrete Sequence Struct?
            // The previous Tick implementation : "g_ModbusData.reg_Freq.f32 = g_Sequence...".
            // So we can just read ModbusData for BOTH Manual and Sequence!
            // This unifies the data path.
            g_Seq_Run = 1; // Enable Sequencer Tick logic
        } else {
            g_Seq_Run = 0; // Disable Sequencer Tick logic
        }

        cmd_freq = g_ModbusData.reg_Freq.f32;
        cmd_vset = g_ModbusData.reg_Vset.f32;
        cmd_voff = g_ModbusData.reg_Voffset.f32;
        cmd_harm3 = g_ModbusData.reg_3rdHarmonic.f32;

        // Limits
        if (cmd_freq > DDS_MAX_FREQ) cmd_freq = DDS_MAX_FREQ;
        if (cmd_freq < 0.0f) cmd_freq = 0.0f;
        if (cmd_vset < 0.0f) cmd_vset = 0.0f; if (cmd_vset > 1.0f) cmd_vset = 1.0f;
        if (cmd_harm3 < 0.0f) cmd_harm3 = 0.0f;

        // Frequency Ramp Trigger
        if (fabsf(cmd_freq - g_Last_Freq_Cmd) > 0.001f) {
            g_Ramp_Freq.start = g_Ramp_Freq.current;
            g_Ramp_Freq.target = cmd_freq;
            if (g_ModbusData.reg_FreqTransTime.f32 <= 0.001f) g_Ramp_Freq.t_duration_ms = 0;
            else g_Ramp_Freq.t_duration_ms = (uint32_t)(g_ModbusData.reg_FreqTransTime.f32 * 1000.0f);
            g_Ramp_Freq.t_elapsed_ms = 0;
            g_Last_Freq_Cmd = cmd_freq;
        }

        float volt_time_sec = g_ModbusData.reg_VoltTransTime.f32;
        uint32_t volt_dur_ms = (volt_time_sec <= 0.001f) ? 0 : (uint32_t)(volt_time_sec * 1000.0f);

        // Vset Ramp Trigger
        if (fabsf(cmd_vset - g_Last_Vset_Cmd) > 0.001f) {
            g_Ramp_Vset.start = g_Ramp_Vset.current;
            g_Ramp_Vset.target = cmd_vset;
            g_Ramp_Vset.t_duration_ms = volt_dur_ms;
            g_Ramp_Vset.t_elapsed_ms = 0;
            g_Last_Vset_Cmd = cmd_vset;
        }

        // Voffset Ramp Trigger
        if (fabsf(cmd_voff - g_Last_Voffset_Cmd) > 0.001f) {
            g_Ramp_Voffset.start = g_Ramp_Voffset.current;
            g_Ramp_Voffset.target = cmd_voff;
            g_Ramp_Voffset.t_duration_ms = volt_dur_ms;
            g_Ramp_Voffset.t_elapsed_ms = 0;
            g_Last_Voffset_Cmd = cmd_voff;
        }

        // Harm3 Ramp Trigger
        if (fabsf(cmd_harm3 - g_Last_Harm3_Cmd) > 0.001f) {
            g_Ramp_Harm3.start = g_Ramp_Harm3.current;
            g_Ramp_Harm3.target = cmd_harm3;
            g_Ramp_Harm3.t_duration_ms = volt_dur_ms;
            g_Ramp_Harm3.t_elapsed_ms = 0;
            g_Last_Harm3_Cmd = cmd_harm3;
        }
    }

    dds.delay_on_ticks = g_ModbusData.reg_Deadtime; 
    dds.delay_off_ticks = g_ModbusData.reg_Deadtime;
    dds.dither_enable = g_ModbusData.reg_DitherEn;
    dds.dither_amount = g_ModbusData.reg_DitherAmt.f32; 
    if (g_ModbusData.reg_WaveType <= 3) dds.wave_type = (DDS_WaveType)g_ModbusData.reg_WaveType;
}

void DDS_Service_Init(void) {
    DDS_Global_Init(); 
    DDS_InitObj(&dds, DDS_ISR_FREQ_HZ);
    
    g_ModbusData.reg_Freq.f32 = DDS_DEFAULT_FREQ; 
    g_ModbusData.reg_Vset.f32 = 0.9f; 
    g_ModbusData.reg_Enable = 0; 
    g_ModbusData.reg_Deadtime = 50;
    g_ModbusData.reg_FreqTransTime.f32 = 1.0f; 
    g_ModbusData.reg_VoltTransTime.f32 = 1.0f; 
    g_ModbusData.reg_PhaseCutOn.f32 = 0.0f;    
    g_ModbusData.reg_PhaseCutOff.f32 = 360.0f; 
    g_ModbusData.reg_PhaseCutEn = 0;           
    g_ModbusData.reg_DitherAmt.f32 = 0.03f;    
    g_ModbusData.reg_SlewRateEn = 1;           
    g_ModbusData.reg_RampEn = 1; 

    // Init Ramps
    g_Ramp_Freq.current = 0.0f; g_Ramp_Freq.target = 0.0f;
    g_Ramp_Vset.current = 0.0f; g_Ramp_Vset.target = 0.0f;
    g_Ramp_Voffset.current = 0.0f; g_Ramp_Voffset.target = 0.0f;
    g_Ramp_Harm3.current = 0.0f; g_Ramp_Harm3.target = 0.0f;
    
    g_Last_Freq_Cmd = DDS_DEFAULT_FREQ; 
    g_Last_Vset_Cmd = 0.0f; g_Last_Voffset_Cmd = 0.0f; g_Last_Harm3_Cmd = 0.0f;
    
    // Default Sequence
    g_Sequence[0] = (Seq_Instruction_t){60.0f, 0.9f, 5000, 1, 1};
    g_Sequence[1] = (Seq_Instruction_t){50.0f, 0.9f, 5000, 2, 1};
    g_Sequence[2] = (Seq_Instruction_t){400.0f, 0.2f, 1000, 3, 1};
    g_Sequence[3] = (Seq_Instruction_t){60.0f, 0.0f, 1000, 0, 0};
}

void DDS_Service_Run_Background(void) {
    static uint32_t seq_timer = 0;

    if (g_Task_1ms_Flag) {
        g_Task_1ms_Flag = 0; 
        
        // Update FSM (Control Logic & Ramping) - Must run at 1kHz
        DDS_Service_UpdateFSM();
        
        // Sequencer Logic
        if (g_Seq_Run && g_ModbusData.reg_Enable) {
             seq_timer++;
             if (seq_timer >= g_Sequence[g_Seq_Index].duration_ms) {
                 seq_timer = 0;
                 // Set Current Params
                 g_ModbusData.reg_Freq.f32 = g_Sequence[g_Seq_Index].freq;
                 g_ModbusData.reg_Vset.f32 = g_Sequence[g_Seq_Index].vset;
                 
                 // Move Next
                 uint16_t next = g_Sequence[g_Seq_Index].next_step_idx;
                 if (next >= SEQ_MAX_STEPS) { g_Seq_Run = 0; } // End
                 else { g_Seq_Index = next; }
             }
        }

        // Ramping Logic
        if (g_ModbusData.reg_RampEn) {
            // Apply S-Curve
            
            // Freq
            if (g_Ramp_Freq.t_elapsed_ms < g_Ramp_Freq.t_duration_ms) {
                g_Ramp_Freq.t_elapsed_ms++;
                float ratio = (float)g_Ramp_Freq.t_elapsed_ms / (float)g_Ramp_Freq.t_duration_ms;
                g_Ramp_Freq.current = Calculate_SCurve(g_Ramp_Freq.start, g_Ramp_Freq.target, ratio);
            } else {
                g_Ramp_Freq.current = g_Ramp_Freq.target;
            }

            // Vset
            if (g_Ramp_Vset.t_elapsed_ms < g_Ramp_Vset.t_duration_ms) {
                g_Ramp_Vset.t_elapsed_ms++;
                float ratio = (float)g_Ramp_Vset.t_elapsed_ms / (float)g_Ramp_Vset.t_duration_ms;
                g_Ramp_Vset.current = Calculate_SCurve(g_Ramp_Vset.start, g_Ramp_Vset.target, ratio);
            } else {
                g_Ramp_Vset.current = g_Ramp_Vset.target;
            }

            // Voffset
            if (g_Ramp_Voffset.t_elapsed_ms < g_Ramp_Voffset.t_duration_ms) {
                g_Ramp_Voffset.t_elapsed_ms++;
                float ratio = (float)g_Ramp_Voffset.t_elapsed_ms / (float)g_Ramp_Voffset.t_duration_ms;
                g_Ramp_Voffset.current = Calculate_SCurve(g_Ramp_Voffset.start, g_Ramp_Voffset.target, ratio);
            } else {
                g_Ramp_Voffset.current = g_Ramp_Voffset.target;
            }

            // Harm3
            if (g_Ramp_Harm3.t_elapsed_ms < g_Ramp_Harm3.t_duration_ms) {
                g_Ramp_Harm3.t_elapsed_ms++;
                float ratio = (float)g_Ramp_Harm3.t_elapsed_ms / (float)g_Ramp_Harm3.t_duration_ms;
                g_Ramp_Harm3.current = Calculate_SCurve(g_Ramp_Harm3.start, g_Ramp_Harm3.target, ratio);
            } else {
                g_Ramp_Harm3.current = g_Ramp_Harm3.target;
            }

        } else {
            // Instant Update
            g_Ramp_Freq.current = g_Ramp_Freq.target;
            g_Ramp_Vset.current = g_Ramp_Vset.target;
            g_Ramp_Voffset.current = g_Ramp_Voffset.target;
            g_Ramp_Harm3.current = g_Ramp_Harm3.target;
        }

        DDS_SetFreq(&dds, g_Ramp_Freq.current, DDS_ISR_FREQ_HZ);
        dds.v_set = g_Ramp_Vset.current;
        dds.v_offset = g_Ramp_Voffset.current; 
        dds.harmonic_3rd_gain = g_Ramp_Harm3.current; 
    }
}

// [NEW] 匯出內部狀態到 Transfer Object (純資料操作，無 IPC 依賴)
void DDS_Service_GetData(DDS_Transfer_t* pOut) {
    pOut->tuning_word = dds.tuning_word;
    pOut->v_set       = dds.v_set;
    pOut->v_offset    = dds.v_offset;
    pOut->wave_type   = (uint16_t)dds.wave_type;
    pOut->enable      = g_ModbusData.reg_Enable;
    
    // Map Advanced Features
    pOut->feature_flags = 0;
    
    if (g_ModbusData.reg_DitherEn) {
        pOut->feature_flags |= DDS_FLAG_DITHER_EN;
    }
    pOut->dither_amount = g_ModbusData.reg_DitherAmt.f32; // [New]
    
    // 3rd Harmonic (Always Enabled flag if value > 0, ramping handles value)
    if (dds.harmonic_3rd_gain > 0.001f) {
        pOut->feature_flags |= DDS_FLAG_3RD_HARM_EN;
        pOut->harmonic_3rd_gain = dds.harmonic_3rd_gain;
    } else {
        pOut->harmonic_3rd_gain = 0.0f;
    }
    
    // Deadtime
    if (g_ModbusData.reg_Deadtime > 0) {
        pOut->feature_flags |= DDS_FLAG_DEADTIME_EN;
        pOut->deadtime_ticks = g_ModbusData.reg_Deadtime;
    } else {
        pOut->deadtime_ticks = 0;
    }
    
    // Slew Rate (Voltage Limit)
    if (g_ModbusData.reg_SlewRateEn) { // [New] Controlled by switch
        pOut->feature_flags |= DDS_FLAG_SLEW_LIMIT_EN; 
        pOut->slew_rate = 10.0f; 
    }

    // Phase Cutting (Dimmer)
    // Only enable if Switch is ON AND angles are valid
    if (g_ModbusData.reg_PhaseCutEn) {
        float on_deg = g_ModbusData.reg_PhaseCutOn.f32;
        float off_deg = g_ModbusData.reg_PhaseCutOff.f32;
        
        // Default safe range
        if(off_deg <= 0.1f) off_deg = 360.0f; 
        
        // Check if cutting is actually needed (optimization)
        if (on_deg > 0.1f || off_deg < 359.9f) {
            pOut->feature_flags |= DDS_FLAG_PHASE_CUT_EN;
            pOut->phase_cut_on = (uint16_t)on_deg;
            pOut->phase_cut_off = (uint16_t)off_deg;
        } else {
            pOut->phase_cut_on = 0;
            pOut->phase_cut_off = 360;
        }
    } else {
        // Disabled: Full Wave
        pOut->phase_cut_on = 0;
        pOut->phase_cut_off = 360;
    }
}

// [NEW] 任意波形寫入 API
void DDS_Service_WriteArbWave(uint16_t index, float value) {
    if (g_ModbusData.reg_Enable == 0) { // Safety: Only allow writing when stopped
        DDS_SetArbValue(&dds, index, value);
    }
}

// [NEW] 序列步驟寫入 API
void DDS_Service_WriteSequence(uint16_t step_idx, float freq, float v_set, uint32_t ms, uint16_t next, uint16_t repeat) {
    if (step_idx < SEQ_MAX_STEPS && g_ModbusData.reg_Enable == 0) { // Safety check
        g_Sequence[step_idx].freq = freq;
        g_Sequence[step_idx].vset = v_set;
        g_Sequence[step_idx].duration_ms = ms;
        g_Sequence[step_idx].next_step_idx = next;
        g_Sequence[step_idx].repeat_count = repeat;
    }
}

// ... Init function remains ...

// [NEW] 閉迴路控制 Stub
void DDS_Service_UpdateClosedLoop(float v_adc_feedback) {
#if DDS_ENABLE_CLOSE_LOOP_STUB
    // TODO: Implement PID or 2p2z controller here
    // float error = g_Vset_Target - v_adc_feedback;
    // ...
#endif
}

// [NEW] CPU1 Timebase Tick (Called by Timer ISR 1ms)
void DDS_Service_Tick(void) {
    g_Task_1ms_Flag = 1;
}

#pragma CODE_SECTION(DDS_Service_Run_ISR, ".TI.ramfunc");
float DDS_Service_Run_ISR(void) {
    // Legacy: used for single-core simulation only.
    // In Dual-Core mode, CPU1 uses Tick(), CPU2 uses dds_core directly.
    static uint16_t tick_cnt = 0; tick_cnt++;
    if (tick_cnt >= DDS_TICKS_PER_MS) { tick_cnt = 0; g_Task_1ms_Flag = 1; }
    
    if (g_ModbusData.reg_Enable == 0 && dds.v_set <= 0.0001f) {
        return 0.0f;
    }

    float out = DDS_Update(&dds); 
    float duty = (out + 1.0f) * 0.5f;
    if (duty > 0.98f) duty = 0.98f; if (duty < 0.02f) duty = 0.02f;
    return duty;
}