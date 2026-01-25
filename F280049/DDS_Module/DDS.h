/*
 * DDS.h
 *
 * Decoupled DDS Component
 * 
 * "I only update parameters, unaware of timing source."
 */

#ifndef DDS_H_
#define DDS_H_

#include <stdint.h>
#include "lut.h"
#include "DDS_UserConfig.h" // Import User Configuration

// --- Portability Wrapper (Intrinsics) ---
#if defined(__TMS320C28XX__) // TI C2000 Compiler
    #define DDS_SAT(val, max, min)  __fsat(val, max, min)
#else // Standard C (Generic)
    #define DDS_SAT(val, max, min)  (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
#endif

// --- Helper Macros ---
#define DDS_BIT_SHIFT       (24)
#define DDS_Q24_ONE         (16777216) // 2^24

// Float -> Q24 Integer Conversion
// e.g. 1.0f -> 16777216
#define DDS_FLOAT_TO_Q24(f)    ((int32_t)((f) * (float)DDS_Q24_ONE))

// Fixed Point Saturation (Standard C)
#define DDS_SAT_I32(val, max, min)  (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

// Note: DDS_PU_TO_STEP is now used on Q24 integers too (Shift logic check needed?)
// Current: ((pu) * DDS_ACC_RANGE)
// If PU is now Q24 (16M), multiplication by ACC_RANGE (4G) would overflow 64-bit if not careful.
// Target: Q24 (1.0) -> u32 (2^32). 
// 16777216 -> 4294967296. Ratio is 256 (2^8).
// So: Step = Q24 << 8.
#define DDS_Q24_TO_STEP(q24)   ((uint32_t)(q24) << 8)


// Readable Unit Conversions (User Unit -> PU Float - ONLY for CMD interface)
#define DDS_HZ_TO_PU(hz)       ((float)(hz) / ISR_FREQ_HZ)
#define DDS_DEG_TO_PU(deg)     ((float)(deg) / 360.0f)
#define DDS_VOLT_TO_PU(volt)   ((float)(volt) / V_BASE)

// --- Type Definitions ---
typedef enum{
    WAVE_DC_ONLY   = (0x0000),
    WAVE_SINE      = (0x0001 << 0),
    WAVE_TRAPEZOID = (0x0001 << 1),
    WAVE_SoftStart  = (0x0001 << 2),
} DDS_WaveType;

typedef union{
    uint32_t u32All;
    struct{
        uint32_t u32Decimal :20;
        uint32_t u32Index   :12;
    };
} DDS_Acc;

// [Refactor] Signed Fixed Point Ramp
typedef struct {
    int32_t    i32Target;   // Goal (Q24)
    int32_t    i32Current;  // Current Position (Q24)
    int32_t    i32MaxStep;  // Max Change per Update (Q24)
} DDS_Ramp;


typedef enum {
    DDS_STATE_STOPPED = 0,
    DDS_STATE_WAIT_ON,
    DDS_STATE_RUNNING,
    DDS_STATE_WAIT_OFF
} DDS_State;


typedef struct {
    DDS_Acc      PhaseAcc;

    uint16_t     u16DAC_Output;
    uint32_t     u32FreqWord;
    uint32_t     u32PhaseWord;

    uint32_t     u32OutScale;            // Q0 Scale (Peak-to-Peak Counts)
    uint32_t     u32OutBias;             // Q0 Bias (Min Value Counts)
    const uint32_t* pLUT;
} DDS_Engine;

typedef struct {
    DDS_WaveType WaveType; // User Config
    
    uint16_t u16CmdEnable;
    uint32_t u32DelayOn_ms;
    uint32_t u32DelayOff_ms;
    
    float    fFreq_Hz;
    float    fAmp_V;
    float    fOffset_V;
    float    fPhase_Deg;
    
    float    fSlewRate_sec; // Time to reach full scale change
} DDS_Cmd;

typedef struct {
    DDS_WaveType WaveType;
    DDS_State    State;
} DDS_History;

typedef struct {
    DDS_Ramp     Freq;    
    DDS_Ramp     Amp;     
    DDS_Ramp     Offset;  
    DDS_Ramp     Phase;   
} DDS_Ramps;


// --- Main DDS Structure ---
typedef struct {

    DDS_Engine Engine;

    // Control Parameters (Using internal Ramp)
    DDS_Ramps    Ramp;
    
    // Internal History
    DDS_History  Last;

    // Status
    DDS_State    State;

    // User Interface
    DDS_Cmd      Cmd;

    uint32_t     u32DelayCounter;
} DDS_Struct;

typedef DDS_Struct* DDS_Handle;

// Global Instance
extern DDS_Struct DDS;

// --- Function Prototypes ---
extern void DDS_Init(void);
extern void DDS_SetSlewTime(DDS_Handle v, float time_sec); // Helper
extern void DDS_Task(void); // Matches SysTick_Task type


// Core Engine (Inline)
// Returns: Raw DAC Code (0-4095)
static inline uint16_t DDS_Run_Core(DDS_Handle v)
{
    // 1. Phase Accumulation
    v->Engine.PhaseAcc.u32All += v->Engine.u32FreqWord;

    // 2. Add Phase Offset
    DDS_Acc TotalACC;
    TotalACC.u32All = v->Engine.PhaseAcc.u32All + v->Engine.u32PhaseWord;

    // 3. Lookup (Q24 Integer)
    uint32_t u32LutVal = v->Engine.pLUT[TotalACC.u32Index];

    // 4. Output Calculation (Fixed Point)
    // Formula: (LUT_Q24 * Scale_Q0) >> 24 + Bias_Q0
    // We use uint64_t cast to prevent 32-bit overflow during multiplication
    // (16M * 4096 = 68 Billion, which fits in 64-bit)
    uint32_t u32Output = (uint32_t)(((uint64_t)u32LutVal * v->Engine.u32OutScale) >> 24) + v->Engine.u32OutBias;

    // 5. Saturation (DAC Limit)
    if(u32Output > 4095) u32Output = 4095;
    
    return (uint16_t)u32Output;
}

#endif /* DDS_H_ */
