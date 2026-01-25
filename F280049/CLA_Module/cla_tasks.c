/*
 * cla_tasks.c
 *
 * CPU-side CLA Control Functions
 */

#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "cla_tasks.h"

/*
 * CLA_Module_Init
 * Initialize CLA module
 */
void CLA_Module_Init(void)
{
    // Initialize shared memory
    CpuToCla1.f32Phase = 0.0f;
    CpuToCla1.f32Modulation = 0.0f;
    CpuToCla1.f32Frequency = 60.0f;
    
    CpuToCla1.f32Target = 0.0f;
    CpuToCla1.f32Feedback = 0.0f;
    CpuToCla1.f32Kp = 1.0f;
    CpuToCla1.f32Ki = 0.1f;
    CpuToCla1.f32Kd = 0.01f;
    
    CpuToCla1.f32Current_A = 0.0f;
    CpuToCla1.f32Current_B = 0.0f;
    CpuToCla1.f32Current_C = 0.0f;
    
    Cla1ToCpu.f32DutyA = 0.5f;
    Cla1ToCpu.f32DutyB = 0.5f;
    Cla1ToCpu.f32DutyC = 0.5f;
    Cla1ToCpu.f32PID_Output = 0.0f;
    Cla1ToCpu.f32PID_Integral = 0.0f;
    Cla1ToCpu.u16TaskComplete = 0;
    
    // Initialize PID state
    g_CLA_PID.f32Integral = 0.0f;
    g_CLA_PID.f32PrevError = 0.0f;
    g_CLA_PID.f32Output = 0.0f;
    
    // Note: CLA hardware initialization is done in Board_init() via SysConfig
}

/*
 * CLA_TriggerSPWM
 * Trigger CLA Task 1 to calculate SPWM duty cycles
 */
void CLA_TriggerSPWM(float phase, float modulation)
{
    // Update input parameters
    CpuToCla1.f32Phase = phase;
    CpuToCla1.f32Modulation = modulation;
    
    // Trigger Task 1
    CLA_forceTasks(CLA1_BASE, CLA_TASKFLAG_1);
}

/*
 * CLA_TriggerPID
 * Trigger CLA Task 2 to calculate PID output
 */
void CLA_TriggerPID(float target, float feedback)
{
    // Update input parameters
    CpuToCla1.f32Target = target;
    CpuToCla1.f32Feedback = feedback;
    
    // Trigger Task 2
    CLA_forceTasks(CLA1_BASE, CLA_TASKFLAG_2);
}

/*
 * CLA_TriggerClarke
 * Trigger CLA Task 3 to perform Clarke transform
 */
void CLA_TriggerClarke(float ia, float ib, float ic)
{
    // Update input parameters
    CpuToCla1.f32Current_A = ia;
    CpuToCla1.f32Current_B = ib;
    CpuToCla1.f32Current_C = ic;
    
    // Trigger Task 3
    CLA_forceTasks(CLA1_BASE, CLA_TASKFLAG_3);
}

/*
 * CLA_ConfigPID
 * Configure PID controller parameters
 */
void CLA_ConfigPID(float kp, float ki, float kd)
{
    CpuToCla1.f32Kp = kp;
    CpuToCla1.f32Ki = ki;
    CpuToCla1.f32Kd = kd;
}

/*
 * CLA_ResetPID
 * Reset PID controller state
 */
void CLA_ResetPID(void)
{
    g_CLA_PID.f32Integral = 0.0f;
    g_CLA_PID.f32PrevError = 0.0f;
    g_CLA_PID.f32Output = 0.0f;
    
    Cla1ToCpu.f32PID_Output = 0.0f;
    Cla1ToCpu.f32PID_Integral = 0.0f;
}
