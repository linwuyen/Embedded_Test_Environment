/*
 * cla_shared.h
 *
 * CPU-CLA Shared Memory Definitions
 */

#ifndef CLA_MODULE_CLA_SHARED_H_
#define CLA_MODULE_CLA_SHARED_H_

#include <stdint.h>

// CPU to CLA Message RAM
typedef struct {
    float f32Phase;             // Current phase angle (0-360 degrees)
    float f32Modulation;        // Modulation index (0-1)
    float f32Frequency;         // Output frequency (Hz)
    
    // PID Control Parameters
    float f32Target;            // Target value
    float f32Feedback;          // Feedback value
    float f32Kp;                // Proportional gain
    float f32Ki;                // Integral gain
    float f32Kd;                // Derivative gain
    
    // Current values for Clarke/Park transform
    float f32Current_A;
    float f32Current_B;
    float f32Current_C;
    
} CpuToCla1_t;

// CLA to CPU Message RAM
typedef struct {
    // SPWM Duty Cycles (0-1)
    float f32DutyA;
    float f32DutyB;
    float f32DutyC;
    
    // PID Output
    float f32PID_Output;
    float f32PID_Integral;
    
    // Clarke/Park Transform Results
    float f32Alpha;             // Alpha axis
    float f32Beta;              // Beta axis
    float f32D;                 // D axis
    float f32Q;                 // Q axis
    
    // Status
    uint16_t u16TaskComplete;
    
} Cla1ToCpu_t;

// PID State (in CLA Data RAM)
typedef struct {
    float f32Integral;
    float f32PrevError;
    float f32Output;
} CLA_PID_t;

// External declarations (must be declared before pragma in header files)
extern CpuToCla1_t CpuToCla1;
extern Cla1ToCpu_t Cla1ToCpu;
extern CLA_PID_t g_CLA_PID;

#endif /* CLA_MODULE_CLA_SHARED_H_ */
