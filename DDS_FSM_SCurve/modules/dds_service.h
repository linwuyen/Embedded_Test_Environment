/**
 * @file dds_service.h
 * @brief DDS 應用服務層 (Tektronix Sequence Edition)
 */
#ifndef DDS_SERVICE_H
#define DDS_SERVICE_H
#include <stdint.h>
#include "dds_core.h" 
#include "dds_defs.h" 

#define DDS_ISR_FREQ_HZ    100000.0f
#define DDS_TICKS_PER_MS   100       

typedef union { float f32; uint16_t u16[2]; } Modbus_Float_t;

typedef struct {
    Modbus_Float_t reg_Freq; Modbus_Float_t reg_Vset; Modbus_Float_t reg_Voffset;
    uint16_t reg_Deadtime; uint16_t reg_WaveType; uint16_t reg_Enable;
    Modbus_Float_t reg_3rdHarmonic; uint16_t reg_DitherEn;
    Modbus_Float_t reg_FreqTransTime; // [NEW] Transition Time in Seconds (4.0 = 4s)
    Modbus_Float_t reg_VoltTransTime; // [NEW] Voltage Transition Time (Seconds)
    Modbus_Float_t reg_PhaseCutOn;    // [NEW] Phase Cut Start Angle (0-360)
    Modbus_Float_t reg_PhaseCutOff;   // [NEW] Phase Cut End Angle (0-360)
    uint16_t reg_PhaseCutEn;          // [NEW] Phase Cut Switch (0=Disable, 1=Enable)
    Modbus_Float_t reg_DitherAmt;     // [NEW] Dither Amount (0.0 - 0.1)
    uint16_t reg_SlewRateEn;          // [NEW] Voltage Slew Rate Limit Switch (0=Bypass, 1=Enable)
    uint16_t reg_RampEn;              // [NEW] Master Ramping Switch (0=Instant, 1=Ramp)
    uint16_t reg_SeqRun; 
    
    // [NEW] Read-Only Status
    uint16_t reg_CurrentState;        // DDS_State_t (0=Idle, 1=RunManual, 2=RunSeq, 3=Fault)
    uint16_t reg_FaultCode;           // 0=None, 1=OVP, 2=OCP
} Modbus_Map_t;

extern volatile Modbus_Map_t g_ModbusData;

// Control State Machine
typedef enum {
    DDS_STATE_IDLE = 0,
    DDS_STATE_RUN_MANUAL = 1,
    DDS_STATE_RUN_SEQUENCE = 2,
    DDS_STATE_FAULT = 3
} DDS_State_t;



typedef struct {
    float    freq;          
    float    vset;          
    uint32_t duration_ms;   
    uint16_t next_step_idx; 
    uint16_t repeat_count;  
} Seq_Instruction_t;

// [NEW] CPU1 Tick Handler (Must be called by a 1kHz Timer ISR)
void DDS_Service_Tick(void);

void DDS_Service_Init(void);
void DDS_Service_Run_Background(void);

// [Optimization] CPU1 Waveform Calculation (Optional, only for single-core or CPU2)
float DDS_Service_Run_ISR(void);

// [NEW] Decoupled API: Outputs data state only. No IPC dependency.
void DDS_Service_GetData(DDS_Transfer_t* pOut);

// [NEW] 任意波形寫入 API (用於 CSV 下載)
// index: 0 ~ 1023, value: -1.0 ~ +1.0
void DDS_Service_WriteArbWave(uint16_t index, float value);

// [NEW] 序列步驟寫入 API
void DDS_Service_WriteSequence(uint16_t step_idx, float freq, float v_set, uint32_t ms, uint16_t next, uint16_t repeat);

// [NEW] Placeholder for future Closed Loop implementation
void DDS_Service_UpdateClosedLoop(float v_adc_feedback);

#endif