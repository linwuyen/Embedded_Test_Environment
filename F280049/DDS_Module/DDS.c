/*
 * DDS.c
 */

#include "DDS.h"


//  (Global Instance)
DDS_Struct DDS;


// --- Initialization ---
void DDS_Init(void)
{
    DDS_Handle v = &DDS;
    
    // 1. User defaults (Physical Units) - "Single Source of Truth"
    v->Cmd.u16CmdEnable   = 0;        // Default OFF
    v->Cmd.fFreq_Hz       = 60.0f;    // 60Hz
    v->Cmd.fAmp_V         = 1.65f;
    v->Cmd.fOffset_V      = 1.65f;    // 1.65V (Mid-rail)
    v->Cmd.fPhase_Deg     = 0.0f;     // 0 deg
    
    v->Cmd.u32DelayOn_ms  = 10;
    v->Cmd.u32DelayOff_ms = 10;
    v->Cmd.fSlewRate_sec  = 1.0f;    // 0.01s Slew Time
    v->Cmd.WaveType       = WAVE_SINE;

    // 2. Initialize Internal States (Derived from Physical Defaults)
    // Convert Float PU to Q24 Integer
    
    // Freq
    // [High-Res Sync] Use same logic as Param_Process
    double stepFactor = 4294967296.0 / (double)ISR_FREQ_HZ;
    v->Ramp.Freq.i32Target  = (int32_t)((v->Cmd.fFreq_Hz * stepFactor) + 0.5);
    v->Ramp.Freq.i32Current = 0; 

    // Amplitude
    v->Ramp.Amp.i32Target   = DDS_FLOAT_TO_Q24(DDS_VOLT_TO_PU(v->Cmd.fAmp_V));
    v->Ramp.Amp.i32Current  = 0; 

    // Offset
    int32_t i32OffsetPU     = DDS_FLOAT_TO_Q24(DDS_VOLT_TO_PU(v->Cmd.fOffset_V));
    v->Ramp.Offset.i32Target  = i32OffsetPU;
    v->Ramp.Offset.i32Current = i32OffsetPU; // Start Offset immediately

    // Phase
    v->Ramp.Phase.i32Target  = 0;
    v->Ramp.Phase.i32Current = 0;
    
    // 3. Status Defaults
    v->u32DelayCounter = 0;
    v->State           = DDS_STATE_STOPPED;
    
    v->Last.WaveType   = WAVE_SINE;
    v->Last.State      = DDS_STATE_STOPPED;
    
    // 4. Engine & Slew Setup
    v->Engine.pLUT     = g_SinLUT_Q24;
    DDS_SetSlewTime(v, v->Cmd.fSlewRate_sec); 
}



void DDS_SetSlewTime(DDS_Handle v, float time_sec)
{
    // Convert time to steps (Still needs basic float math for "User Time" -> "Steps")
    // But we settle to Integer quickly.
    float steps = time_sec * ISR_OUTPUT_HZ;
    if (steps < 1.0f) steps = 1.0f;
    
    // Rate = 1.0(Q24) / Steps
    // i32MaxStep = 16777216 / steps
    int32_t i32Rate = (int32_t)((float)DDS_Q24_ONE / steps);
    
    // Freq uses specialized 32-bit resolution (Raw Steps), so its scale is 256x larger (Q24->Q32)
    // 1.0 PU (freq) = 2^32. 1.0 PU (others) = 2^24.
    // Check for overflow: if i32Rate is large, << 8 will overflow int32.
    // 0x01000000 (Q24_ONE) << 8 is 0 (Overflow).
    if (i32Rate > 8388607) // 0x7FFFFF
    {
         v->Ramp.Freq.i32MaxStep = 2147483647; // Max S32
    }
    else
    {
         v->Ramp.Freq.i32MaxStep   = i32Rate << 8; 
    }
    
    v->Ramp.Amp.i32MaxStep    = i32Rate;
    v->Ramp.Offset.i32MaxStep = i32Rate;
}


static int DDS_Ramp_Update(DDS_Ramp *p, int32_t Max, int32_t Min)
{
    int32_t i32Old = p->i32Current;
    int32_t i32Delta = p->i32Target - p->i32Current;

    // 1. Calculate Step with Saturation (Rate Limiter)
    int32_t i32Step = DDS_SAT_I32(i32Delta, p->i32MaxStep, -p->i32MaxStep);

    // 2. Apply Step & Clamp Result (Range Limiter)
    // Careful with Overflow if Current + Step > 32bit max, but Q24 usually fits.
    p->i32Current = DDS_SAT_I32(p->i32Current + i32Step, Max, Min);

    return (p->i32Current != i32Old);
}



// --- Internal Implementation ---

static void DDS_Sequence_Update(DDS_Handle v)
{
    // 1. Timer Logic: Run counter only in transitional states
    // This removes the need to increment counter in every case block
    int bIsWaiting = (v->State == DDS_STATE_WAIT_ON || v->State == DDS_STATE_WAIT_OFF);
    
    if (bIsWaiting) v->u32DelayCounter++;
    else            v->u32DelayCounter = 0;

    // 2. Transition Logic (State Machine)
    switch (v->State)
    {
    case DDS_STATE_STOPPED:  // 0
        if (v->Cmd.u16CmdEnable) v->State = DDS_STATE_WAIT_ON;
        break;

    case DDS_STATE_WAIT_ON:  // 1
        if (!v->Cmd.u16CmdEnable)                            v->State = DDS_STATE_WAIT_OFF; // Abort start
        else if (v->u32DelayCounter >= v->Cmd.u32DelayOn_ms) v->State = DDS_STATE_RUNNING;  // Time's up
        break;

    case DDS_STATE_RUNNING:  // 2
        if (!v->Cmd.u16CmdEnable) v->State = DDS_STATE_WAIT_OFF;
        break;

    case DDS_STATE_WAIT_OFF: // 3
        if (v->Cmd.u16CmdEnable)                              v->State = DDS_STATE_WAIT_ON; // Restart
        else if (v->u32DelayCounter >= v->Cmd.u32DelayOff_ms) v->State = DDS_STATE_STOPPED; // Time's up
        break;
        
    default:
        v->State = DDS_STATE_STOPPED;
        break;
    }
}

static int DDS_Param_Process(DDS_Handle v)
{
    // 0.5 Update Targets from Cmd (Convert Float to Q24 Int)
    
    // [High-Res Frequency Update]
    // Calculate raw 32-bit phase step directly from Double Precision Math.
    // Step = (Freq / ISR_FREQ) * 2^32
    // Factor = 4294967296.0 / ISR_FREQ_HZ
    // This provides ~0.000028 Hz resolution.
    double stepFactor = 4294967296.0 / (double)ISR_FREQ_HZ;
    v->Ramp.Freq.i32Target = (int32_t)(v->Cmd.fFreq_Hz * stepFactor);

    v->Ramp.Phase.i32Target   = DDS_FLOAT_TO_Q24(DDS_DEG_TO_PU(v->Cmd.fPhase_Deg));
    v->Ramp.Offset.i32Target  = DDS_FLOAT_TO_Q24(DDS_VOLT_TO_PU(v->Cmd.fOffset_V));
    
    // [Soft Stop Feature]
    if (v->State == DDS_STATE_WAIT_OFF)
    {
        v->Ramp.Amp.i32Target = 0;
    }
    else
    {
        v->Ramp.Amp.i32Target = DDS_FLOAT_TO_Q24(DDS_VOLT_TO_PU(v->Cmd.fAmp_V));
    }
    
    // Dynamic Slew Rate Update
    DDS_SetSlewTime(v, v->Cmd.fSlewRate_sec);
    
    // [Smart Config Linkage]
    uint32_t minDelayOff = (uint32_t)(v->Cmd.fSlewRate_sec * 1000.0f) + 100;
    if (v->Cmd.u32DelayOff_ms < minDelayOff)
    {
        v->Cmd.u32DelayOff_ms = minDelayOff;
    }

    // 1. Slew Rate Update (All Integers)
    int isChanged = 0;
    
    // Freq Max: DDS_MAX_FREQ_HZ converted to Raw Steps (32-bit)
    // Re-use the stepFactor calc from above or macro
    // stepFactor is already calculated above
    int32_t i32MaxFreq = (int32_t)(DDS_MAX_FREQ_HZ * stepFactor);
    
    isChanged |= DDS_Ramp_Update(&v->Ramp.Freq,   i32MaxFreq, 0);
    isChanged |= DDS_Ramp_Update(&v->Ramp.Phase,  DDS_Q24_ONE, 0); // 1.0 PU
    isChanged |= DDS_Ramp_Update(&v->Ramp.Amp,    DDS_Q24_ONE, 0);
    isChanged |= DDS_Ramp_Update(&v->Ramp.Offset, DDS_Q24_ONE, 0);

    // [Soft Start Feature]
    if (v->State == DDS_STATE_STOPPED || v->State == DDS_STATE_WAIT_ON)
    {
        v->Ramp.Amp.i32Current = 0;
    }

    if (v->Cmd.WaveType != v->Last.WaveType)
    {
        isChanged = 1;
        v->Last.WaveType = v->Cmd.WaveType;
    }
    
    if (v->State != v->Last.State)
    {
        isChanged = 1;
        v->Last.State = v->State;
    }

    return isChanged;
}



static void DDS_Engine_Update(DDS_Handle v, int isChanged)
{
    if (!isChanged) return;

    // --- 1. Raw Conversion (Q24 -> Step) ---
    // Frequency is now natively 32-bit Raw Step, no shift needed.
    uint32_t stepFreq  = (uint32_t)v->Ramp.Freq.i32Current;
    
    // Phase is still Q24, needs shift.
    // Shift Q24 up by 8 bits
    uint32_t stepPhase = DDS_Q24_TO_STEP(v->Ramp.Phase.i32Current);

    // --- 2. Standard Output Calc (AC Mode) ---
    // Scale Logic:
    // Scale = Amp(Q24) * 2 * DAC_FULL
    // We cast to u64 to avoid overflow then down to u32
    // Or we keep ratio: Amp(Q24)/Q24_ONE = Amp(PU).
    // Target: Amp(PU) * DAC_FULL.
    // So: Amp(Q24) * DAC_FULL / Q24_ONE?
    // No, Scale in Run_Core is used as: (LUT_Q24 * Scale_Q0) >> 24
    // If Scale is DAC_FULL (e.g. 4095), then Output = (1.0 * 4095) = 4095. Correct.
    // So "Scale" MUST BE the total DAC Counts peak-to-peak.
    // Calculation:
    // Amp(Q24) represents e.g. 0.5 (8388608). We want 0.5 * 2 * 4095 = 4095.
    // (8388608 * 2 * 4095) / 16777216 = 4095.
    // Formula: (Amp_Q24 * 2 * DAC_FULL) >> 24
    
    uint64_t u64Amp = v->Ramp.Amp.i32Current;
    uint32_t u32Scale = (uint32_t)((u64Amp * 2 * (uint32_t)DAC_FULL_SCALE) >> 24);

    // Bias Logic: (Offset_Q24 - Amp_Q24) * DAC_FULL / Q24_ONE
    int32_t i32BiasQ24 = v->Ramp.Offset.i32Current - v->Ramp.Amp.i32Current;
    if (i32BiasQ24 < 0) i32BiasQ24 = 0;
    
    uint32_t u32Bias = (uint32_t)(((uint64_t)i32BiasQ24 * (uint32_t)DAC_FULL_SCALE) >> 24);

    // --- 3. LUT Selection ---
    const uint32_t* pLUT = g_SinLUT_Q24;
    if (v->Cmd.WaveType == WAVE_TRAPEZOID) pLUT = g_TeapLUT_Q24;


    // --- 4. Overrides & Safety ---
    int bGenActive = (v->State == DDS_STATE_RUNNING || v->State == DDS_STATE_WAIT_OFF);
    int bForceDC   = (v->Cmd.WaveType == WAVE_DC_ONLY || v->Ramp.Freq.i32Current <= 0);

    if (!bGenActive)
    {
        // Case: System Stopped (Zero Output, Reset Phase)
        u32Scale = 0;
        // Bias = Offset * DAC_FULL
        u32Bias  = (uint32_t)(((uint64_t)v->Ramp.Offset.i32Current * (uint32_t)DAC_FULL_SCALE) >> 24);
        v->Engine.PhaseAcc.u32All = 0;
        stepFreq = 0; 
    }
    else if (bForceDC)
    {
        // Case: DC Only Mode
        u32Scale = 0;
        u32Bias  = (uint32_t)(((uint64_t)v->Ramp.Offset.i32Current * (uint32_t)DAC_FULL_SCALE) >> 24);
        v->Engine.PhaseAcc.u32All = 0;
    }

    // --- 5. Commit to Engine ---
    v->Engine.u32FreqWord  = stepFreq;
    v->Engine.u32PhaseWord = stepPhase;
    v->Engine.pLUT         = pLUT;
    v->Engine.u32OutScale  = u32Scale;
    v->Engine.u32OutBias   = u32Bias;
}


void DDS_Task(void)
{
    DDS_Handle v = &DDS;

    // 1. Sequence Management (Start/Stop delays)
    DDS_Sequence_Update(v);

    // 2. Parameter Processing
    int isChanged = DDS_Param_Process(v);

    // 3. Engine Update
    DDS_Engine_Update(v, isChanged);

    // 4. Debug Output (Optional  Hook)
    // Output '1' when Command is Enabled (Trigger signal for Delay measurement)
    // DDS_DEBUG_GPIO(v->Cmd.u16CmdEnable); // <--- COMMENT THIS OUT! It fights with ISR.
}



