/*
 * diag_crystal.h
 *
 * Diagnostic Module Interface
 */

#ifndef DIAGNOSTIC_DIAG_CRYSTAL_H_
#define DIAGNOSTIC_DIAG_CRYSTAL_H_

#include <stdint.h>

// Diagnostic Modes
typedef enum {
    DIAG_MODE_NORMAL = 0,
    DIAG_MODE_CRYSTAL_TEST,
    DIAG_MODE_PWM_TEST,
    DIAG_MODE_ADC_TEST,
    DIAG_MODE_CLA_TEST
} DiagMode_t;

// Diagnostic Status
typedef struct {
    DiagMode_t eMode;
    uint16_t u16TestRunning :1;
    uint16_t u16TestPassed :1;
    uint16_t u16Reserved :14;
} Diag_Status_t;

// Global Diagnostic Status
extern Diag_Status_t g_Diag_Status;

// Function Prototypes
extern void Diag_Init(void);
extern void Diag_SetMode(DiagMode_t mode);
extern void Diag_CrystalTest(void);
extern void Diag_PWMTest(void);
extern void Diag_ADCTest(void);
extern void Diag_CLATest(void);
extern void Diag_ExitTest(void);

#endif /* DIAGNOSTIC_DIAG_CRYSTAL_H_ */
