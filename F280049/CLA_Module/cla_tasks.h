/*
 * cla_tasks.h
 *
 * CLA Tasks Interface
 */

#ifndef CLA_MODULE_CLA_TASKS_H_
#define CLA_MODULE_CLA_TASKS_H_

#include "cla_shared.h"

// CLA Task Function Prototypes
__interrupt void Cla1Task1(void);  // SPWM Calculation
__interrupt void Cla1Task2(void);  // PID Controller
__interrupt void Cla1Task3(void);  // Clarke Transform
__interrupt void Cla1Task4(void);  // Park Transform

// CPU-side Functions
extern void CLA_Module_Init(void);
extern void CLA_TriggerSPWM(float phase, float modulation);
extern void CLA_TriggerPID(float target, float feedback);
extern void CLA_TriggerClarke(float ia, float ib, float ic);

// Helper Functions (CPU-side)
extern void CLA_ConfigPID(float kp, float ki, float kd);
extern void CLA_ResetPID(void);

#endif /* CLA_MODULE_CLA_TASKS_H_ */
