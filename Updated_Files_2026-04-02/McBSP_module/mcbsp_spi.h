#ifndef MCBSP_SPI_H
#define MCBSP_SPI_H

#include "driverlib.h"
#include "device.h"
#include "board.h"

#ifndef myMCBSP0_BASE
/**
 * @brief Default base address for McBSP module if not defined by board.h
 */
#define myMCBSP0_BASE MCBSPA_BASE
#endif

/**
 * @brief Macro for precise NOP delay using RPT instruction
 */
#define MCBSP_CYCLE_NOP0(n)  __asm(" RPT #(" #n ") || NOP")
#define MCBSP_CYCLE_NOP(n)   MCBSP_CYCLE_NOP0(n)

/**
 * @brief Global variables for FSI simulation (shared with DMA)
 */
extern volatile uint16_t Simulated_FSI_RX_Data;
extern volatile uint16_t Simulated_FSI_TX_Data;

/**
 * @brief Initializes McBSP in SPI Master mode with safety startup sequence
 * @param base Base address of the McBSP module
 */
void Init_McBSP_SPI_Master(uint32_t base);

/**
 * @brief Initializes DMA channels for automated control loop compensation
 * @param mcbsp_base Base address of the McBSP module for register mapping
 */
void Init_DMA_CompensationLoop(uint32_t mcbsp_base);

/**
 * @brief Initializes EPWM1 to generate 100kHz SOCA trigger and output signal on GPIO0
 */
void Init_EPWM1_SOCA(void);

#endif // MCBSP_SPI_H
