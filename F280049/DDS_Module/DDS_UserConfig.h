/*
 * DDS_UserConfig.h
 *
 * User Configuration for DDS Module
 * Isolate system-specific constants here for portability.
 */

#ifndef DDS_USER_CONFIG_H_
#define DDS_USER_CONFIG_H_

#define myBoardLED0_GPIO 31

// --- System Parameters
// Hardware Configuration (Must match SysConfig)
#define SYS_CLK_HZ          (100000000.0f)      // 100 MHz
#define EPWM_CLK_DIV        (2.0f)              // TBCLK = SYSCLK / 2
#define EPWM_PERIOD_CNT     (208)               // Timer Period (Up-Down Mode)

// Calculated Frequency
// Freq = TBCLK / (2 * Period) for Up-Down Mode
// Calculated Frequency
// Freq = TBCLK / (2 * Period) for Up-Down Mode
// Use the exact formula to match Hardware Physics (120192.307...)
//#define ISR_FREQ_HZ         ((SYS_CLK_HZ / EPWM_CLK_DIV) / (2.0f * (float)EPWM_PERIOD_CNT))
 #define ISR_FREQ_HZ          120000.0f
#define ISR_OUTPUT_HZ       (1000.0f)

// Maximum Output Frequency (Safety Limit)
#define DDS_MAX_FREQ_HZ   2000.0f


// --- Hardware Scaling ---
// Phase Accumulator Depth (2^32)
#define DDS_ACC_RANGE     4294967296.0f

// System Voltage Reference (For Unit Conversion)
#define V_BASE            3.3f

// DAC Resolution (For Raw Output)
#define DAC_FULL_SCALE    4095.0f


// --- Debug / Integration Hooks ---
// Define this macro to map internal state to a GPIO (e.g., for verifying delays)
// Example: #define DDS_DEBUG_GPIO(x)  GPIO_writePin(34, (x))
#ifndef DDS_DEBUG_GPIO
#include "driverlib.h" // Ensure GPIO_writePin is visible
#define DDS_DEBUG_GPIO(x)    GPIO_writePin(myBoardLED0_GPIO, (x)) 
#endif

#endif /* DDS_USER_CONFIG_H_ */
