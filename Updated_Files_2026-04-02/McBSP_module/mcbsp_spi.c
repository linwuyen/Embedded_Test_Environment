#include "mcbsp_spi.h"

/**
 * @brief Global variables for FSI simulation (shared with DMA)
 */
volatile uint16_t Simulated_FSI_RX_Data = 0x0000;
volatile uint16_t Simulated_FSI_TX_Data = 0x0000;

/**
 * @brief Initializes McBSP in SPI Master mode with safety startup sequence
 * @details Implements 16-bit word length, 2.5MHz SPI clock, and XSYNCERR cleaning procedure
 * @param base Base address of the McBSP module
 */
void Init_McBSP_SPI_Master(uint32_t base)
{
    // Rule 2.1: Force reset McBSP state machines (TX, RX, SRG, FrameSync)
    McBSP_resetTransmitter(base);
    McBSP_resetReceiver(base);
    McBSP_resetSampleRateGenerator(base);
    McBSP_resetFrameSyncLogic(base);

    McBSP_SPIMasterModeParams spiMasterParams;

    // Disable digital loopback, use external 6-pin physical wiring
    spiMasterParams.loopbackModeFlag = false;

    // Rule 2.3: Generate 2.5MHz SPI clock
    // LSPCLK 50MHz / 8 = 25MHz
    spiMasterParams.clockSRGDivider  = 7;

    // Rule 2.2: SPI Parameter Alignment
    // Single-word transfer without delay (CPHA=0)
    spiMasterParams.clockStopMode    = MCBSP_CLOCK_SPI_MODE_NO_DELAY;
    // Transmit on rising edge (CPOL=0)
    spiMasterParams.spiMode          = MCBSP_TX_POLARITY_RISING_EDGE;
    // 16-bit word length
    spiMasterParams.wordLength       = MCBSP_BITS_PER_WORD_16;

    // Configure SPI Master core parameters
    McBSP_configureSPIMasterMode(base, &spiMasterParams);

    // Rule 2.4: XSYNCERR cleaning proceduref
    // 1. Enable SRG
    McBSP_enableSampleRateGenerator(base);
    
    // Wait 8 NOP for SRG stabilization
    MCBSP_CYCLE_NOP(8);

    // 2. Enable TX to catch potential XSYNCERR
    McBSP_enableTransmitter(base);
    
    // Wait for 160 NOPs for synchronization recovery
    MCBSP_CYCLE_NOP(160);

    // 3. Reset TX to clear XSYNCERR
    McBSP_resetTransmitter(base);

    // 4. Safe start for TX, RX, and internal Frame Sync Logic
    McBSP_enableTransmitter(base);
    McBSP_enableReceiver(base);
    McBSP_enableFrameSyncLogic(base);

    // Brief delay after startup
    MCBSP_CYCLE_NOP(8);
}

/**
 * @brief Initializes DMA channels for automated control loop compensation
 * @details Configures CH1 for TX (triggered by EPWM1SOCA) and CH2 for RX (triggered by McBSP REVT)
 * @param mcbsp_base Base address of the McBSP module for register mapping
 */
void Init_DMA_CompensationLoop(uint32_t mcbsp_base)
{
    // Initialize DMA controller and set free run mode for debugging
    DMA_initController();
    DMA_setEmulationMode(DMA_EMULATION_FREE_RUN);

    // Address mapping for registers
    const void *val_Fsi_Rx_Addr = (const void *)&Simulated_FSI_RX_Data;
    const void *reg_DXR1        = (const void *)(mcbsp_base + MCBSP_O_DXR1);

    const void *reg_DRR1        = (const void *)(mcbsp_base + MCBSP_O_DRR1);
    const void *val_Fsi_Tx_Addr = (const void *)&Simulated_FSI_TX_Data;

    // ---------------------------------------------------------
    // TX DMA (CH1): Triggered by PWM to push SPI data
    // ---------------------------------------------------------
    DMA_configAddresses(DMA_CH1_BASE, reg_DXR1, val_Fsi_Rx_Addr);
    // Burst=1 word, fixed src/dest addresses
    DMA_configBurst(DMA_CH1_BASE, 1U, 0, 0);       
    DMA_configTransfer(DMA_CH1_BASE, 1U, 0, 0);    
    // Trigger: EPWM1SOCA, Continuous mode, hardware only
    DMA_configMode(DMA_CH1_BASE, DMA_TRIGGER_EPWM1SOCA, 
                  (DMA_CFG_ONESHOT_DISABLE | DMA_CFG_CONTINUOUS_ENABLE | DMA_CFG_SIZE_16BIT));
    
    DMA_disableInterrupt(DMA_CH1_BASE);
    DMA_enableTrigger(DMA_CH1_BASE);
    DMA_startChannel(DMA_CH1_BASE);

    // ---------------------------------------------------------
    // RX DMA (CH2): Retrieve clock-in data after SPI completion
    // ---------------------------------------------------------
    DMA_configAddresses(DMA_CH2_BASE, val_Fsi_Tx_Addr, reg_DRR1);
    DMA_configBurst(DMA_CH2_BASE, 1U, 0, 0);
    DMA_configTransfer(DMA_CH2_BASE, 1U, 0, 0);
    // Trigger: MCBSP_A REVT
    DMA_configMode(DMA_CH2_BASE, DMA_TRIGGER_MCBSPAMREVT, 
                  (DMA_CFG_ONESHOT_DISABLE | DMA_CFG_CONTINUOUS_ENABLE | DMA_CFG_SIZE_16BIT));
    
    DMA_disableInterrupt(DMA_CH2_BASE);
    DMA_enableTrigger(DMA_CH2_BASE);
    DMA_startChannel(DMA_CH2_BASE);
}

/**
 * @brief Initializes EPWM1 to generate 100kHz SOCA trigger and output signal on GPIO0
 */
void Init_EPWM1_SOCA(void)
{
    // Enable EPWM1 Clock
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM1);

    EALLOW;
    
    // Configure GPIO0 as EPWM1A for measurement
    GPIO_setPinConfig(GPIO_0_EPWM1A);
    GPIO_setPadConfig(0, GPIO_PIN_TYPE_STD);

    // Disable SOCA while configuring
    EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);

    // Time Base Submodule: Set Period for 100kHz
    // TBCLK = EPWMCLK (100MHz)
    // For 100kHz in Up-Down mode: Period = 100MHz / (2 * 100kHz) = 500
    EPWM_setTimeBasePeriod(EPWM1_BASE, 500U);
    EPWM_setPhaseShift(EPWM1_BASE, 0U);
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0U);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);

    // Action Qualifier: Toggle EPWM1A at Zero/Period for 50% duty square wave
    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);

    // Event Trigger: Generate SOCA on Zero event
    EPWM_setADCTriggerSource(EPWM1_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_ZERO);
    EPWM_setADCTriggerEventPrescale(EPWM1_BASE, EPWM_SOC_A, 1U);
    EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_A);

    EDIS;
}
