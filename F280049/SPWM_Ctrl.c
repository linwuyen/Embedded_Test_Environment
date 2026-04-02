/*
 * SPWM_Ctrl.c
 *
 *  Created on: May 27, 2024
 *      Author: roger_lin
 */
#include "common.h"
#include "cTimeMeas.h"

ST_TIMER_MEAS TimerTX = {0};
ST_TIMER_MEAS TimerRX = {0};
////===============================IO=======================

typedef struct{
    uint32_t b0_pfc_ovp_n:1;
    uint32_t b1_pfc_uvp_n :1;
    uint32_t b2_pfc_ovp_2_n:1;
    uint32_t b3_pfc_uvp_2_n:1;
    uint32_t b4_pfc_otp_n:1;
    uint32_t b5_dc_ovp_n:1;
    uint32_t b6_ac_fail_5_n:1;

    uint32_t b7_fan_fail_1:1;
    uint32_t b8_fan_fail_2:1;
    uint32_t b9_fan_fail_3:1;
    uint32_t b10_fan_fail_4:1;
}st_Fake_singal;
st_Fake_singal Fake_singal;

ST_GSVAR sGbVars;


void scanFanFail(void)
{
    static uint16_t u16FanIo = 0;
    static uint16_t u16FanShadow = 0;
    static uint16_t u16Fan1Timeout = 0;
    static uint16_t u16Fan2Timeout = 0;
    static uint16_t u16Fan3Timeout = 0;
    static uint16_t u16Fan4Timeout = 0;

//    u16FanIo  = (GPIO_readPin(FANFAIL1)<<0);
//    u16FanIo |= (GPIO_readPin(FANFAIL2)<<1);
//    u16FanIo |= (GPIO_readPin(FANFAIL3)<<2);
//    u16FanIo |= (GPIO_readPin(FANFAIL4)<<3);

    u16FanIo++;
    u16FanIo &= 0x0F;

    if((u16FanShadow^u16FanIo) & (0x0001<<0)) {
        u16Fan1Timeout = 0;
        sParams.sDin.i_stat.b7_fan_fail_1 = 0;
    }
    else if(u16Fan1Timeout<1000){
        u16Fan1Timeout++;
    }
    else {
        sParams.sDin.i_stat.b7_fan_fail_1 = 1;
    }

    if((u16FanShadow^u16FanIo) & (0x0001<<1)) {
        u16Fan2Timeout = 0;
        sParams.sDin.i_stat.b8_fan_fail_2 = 0;
    }
    else if(u16Fan2Timeout<1000){
        u16Fan2Timeout++;
    }
    else {
        sParams.sDin.i_stat.b8_fan_fail_2 = 1;
    }

    if((u16FanShadow^u16FanIo) & (0x0001<<2)) {
        u16Fan3Timeout = 0;
        sParams.sDin.i_stat.b9_fan_fail_3 = 0;
    }
    else if(u16Fan3Timeout<1000){
        u16Fan3Timeout++;
    }
    else {
        sParams.sDin.i_stat.b9_fan_fail_3 = 1;
    }

    if((u16FanShadow^u16FanIo) & (0x0001<<3)) {
        u16Fan4Timeout = 0;
        sParams.sDin.i_stat.b10_fan_fail_4 = 0;
    }
    else if(u16Fan4Timeout<1000){
        u16Fan4Timeout++;
    }
    else {
        sParams.sDin.i_stat.b10_fan_fail_4 = 1;
    }

    u16FanShadow = u16FanIo;
}

void Update_IO(void)
{

#if (SCAN_ERROR_MODE == DISABLE_SCAN_ERROR)
    sParams.sDin.i_stat.b0_pfc_ovp_n   = 1;
    sParams.sDin.i_stat.b1_pfc_uvp_n   = 1;
    sParams.sDin.i_stat.b2_pfc_ovp_2_n = 0;
    sParams.sDin.i_stat.b3_pfc_uvp_2_n = 0;
    sParams.sDin.i_stat.b4_pfc_otp_n   = 1;
    sParams.sDin.i_stat.b5_dc_ovp_n    = 0;
    sParams.sDin.i_stat.b6_ac_fail_5_n = 1;
    sParams.sDin.i_stat.b7_fan_fail_1  = 0;
    sParams.sDin.i_stat.b8_fan_fail_2  = 0;
    sParams.sDin.i_stat.b9_fan_fail_3  = 0;
    sParams.sDin.i_stat.b10_fan_fail_4 = 0;
#elif (SCAN_ERROR_MODE == FEED_FACK_ERROR)
    sParams.sDin.i_stat.b0_pfc_ovp_n   = Fake_singal.b0_pfc_ovp_n;
    sParams.sDin.i_stat.b1_pfc_uvp_n   = Fake_singal.b1_pfc_uvp_n;
    sParams.sDin.i_stat.b2_pfc_ovp_2_n = Fake_singal.b2_pfc_ovp_2_n;
    sParams.sDin.i_stat.b3_pfc_uvp_2_n = Fake_singal.b3_pfc_uvp_2_n;
    sParams.sDin.i_stat.b4_pfc_otp_n   = Fake_singal.b4_pfc_otp_n;
    sParams.sDin.i_stat.b5_dc_ovp_n    = Fake_singal.b5_dc_ovp_n;
    sParams.sDin.i_stat.b6_ac_fail_5_n   = Fake_singal.b6_ac_fail_5_n;
    sParams.sDin.i_stat.b7_fan_fail_1  = Fake_singal.b7_fan_fail_1;
    sParams.sDin.i_stat.b8_fan_fail_2  = Fake_singal.b8_fan_fail_2;
    sParams.sDin.i_stat.b9_fan_fail_3  = Fake_singal.b9_fan_fail_3;
    sParams.sDin.i_stat.b10_fan_fail_4 = Fake_singal.b10_fan_fail_4;
#else
    sParams.sDin.i_stat.b0_pfc_ovp_n    = GPIO_readPin(PFC_OVP);
    sParams.sDin.i_stat.b1_pfc_uvp_n    = GPIO_readPin(PFC_UVP);
    sParams.sDin.i_stat.b2_pfc_ovp_2_n  = GPIO_readPin(PFC_OVP2);
    sParams.sDin.i_stat.b3_pfc_uvp_2_n  = GPIO_readPin(PFC_UVP2);
    sParams.sDin.i_stat.b4_pfc_otp_n    = GPIO_readPin(PFC_OTP);
    sParams.sDin.i_stat.b5_dc_ovp_n     = GPIO_readPin(DCOVP);
    sParams.sDin.i_stat.b6_ac_fail_5_n  = GPIO_readPin(ACFAIL5);


    scanFanFail();
#endif

   sParams.sDin.i_stat.b16_off_pwm_n  = sParams.sDout.o_stat.b16_off_pwm_n;
   sParams.sDin.i_stat.b17_master  = sParams.sDout.o_stat.b17_master;


   GPIO_writePin(OFF_PWMn  ,sParams.sDout.o_stat.b16_off_pwm_n);
   GPIO_writePin(EN_MASTER ,sParams.sDout.o_stat.b17_master);
}


//==============================ADC_LOOKUPTABLE===================================
#define ADC_MAX 4096
#define lookupTableSize  33
uint16 dutyCycle ;

int16_t TableAdc2DutyCnt[lookupTableSize] = {
  409, 409, 409, 409, 839, 1269, 1700, 1700,
  1700, 1700, 1700, 1700, 1700, 1700, 1700, 1700,
  1700, 1700, 1700, 1700, 1700, 1700, 1700, 1700,
  1700, 1700, 1700, 1700, 1700, 1700, 1700, 1700, 1700
};


int16_t getAdc2DutyCnt(uint16_t adcValue) {
    float scale = (float)(lookupTableSize - 1) / ADC_MAX;
    float indexFloat = adcValue * scale;
    int16_t index = (int16_t)indexFloat;
    int16_t nextIndex = (index + 1 < lookupTableSize) ? index + 1 : index;

    int16_t value1 = TableAdc2DutyCnt[index];
    int16_t value2 = TableAdc2DutyCnt[nextIndex];

    float fraction = indexFloat - index;
    int16_t DutyCnt = value1 + (int16_t)((value2 - value1) * fraction);

    return DutyCnt;
}

uint16 cnvAdc2Duty() {

 //MCP3301 TEMP
   dutyCycle = getAdc2DutyCnt(sGbVars.u16TempC_AD);

   return dutyCycle;
}

////===============================PWM=============================

typedef struct
{
    uint32_t u32BASE;
    uint16_t u16TBperiodCount;
    uint16_t u16count;
    uint16_t u16phaseCount;
    uint16_t u16CompCount;
} sPWM;
typedef sPWM *HAL_sPWM;

//100K 50 Duty 10us
sPWM CLK0 = { .u32BASE = EPWM1_BASE, .u16TBperiodCount = 500, .u16CompCount =
                      250, };

sPWM CLK180 = { .u32BASE = EPWM2_BASE, .u16TBperiodCount = 500, .u16CompCount =
                        250,
                .u16phaseCount = 500, };

//25k 40us
sPWM FANCTRL5 = { .u32BASE = EPWM3_BASE, .u16TBperiodCount = 2000,
                  .u16CompCount = 0, .u16phaseCount = 0 };


void Setup_EPWM(HAL_sPWM p)
{
//TB
    EPWM_setClockPrescaler(p->u32BASE, EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBasePeriod(p->u32BASE, p->u16TBperiodCount);
    EPWM_setTimeBaseCounter(p->u32BASE, p->u16count);
    EPWM_setTimeBaseCounterMode(p->u32BASE, EPWM_COUNTER_MODE_UP_DOWN);

//phase
    EPWM_forceSyncPulse(CLK0_5_BASE);
    EPWM_enablePhaseShiftLoad(CLK180_5_BASE);
    EPWM_setPhaseShift(p->u32BASE, p->u16phaseCount);

//CC
    EPWM_setCounterCompareValue(p->u32BASE, EPWM_COUNTER_COMPARE_A,
                                p->u16CompCount);
    EPWM_setCounterCompareShadowLoadMode(p->u32BASE, EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareValue(p->u32BASE, EPWM_COUNTER_COMPARE_B,
                                p->u16CompCount);
    EPWM_setCounterCompareShadowLoadMode(p->u32BASE, EPWM_COUNTER_COMPARE_B,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

//AQ-A
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_NO_CHANGE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_NO_CHANGE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_NO_CHANGE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_NO_CHANGE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
//AQ-B
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_NO_CHANGE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_NO_CHANGE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_NO_CHANGE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(p->u32BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_NO_CHANGE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
//DB

    EPWM_setRisingEdgeDelayCountShadowLoadMode(p->u32BASE,
                                               EPWM_RED_LOAD_ON_CNTR_ZERO);
    EPWM_disableRisingEdgeDelayCountShadowLoadMode(p->u32BASE);
    EPWM_setFallingEdgeDelayCountShadowLoadMode(p->u32BASE,
                                                EPWM_FED_LOAD_ON_CNTR_ZERO);
    EPWM_disableFallingEdgeDelayCountShadowLoadMode(p->u32BASE);
}




void updateFANCTRL5Duty( void)
{
    EPWM_setCounterCompareValue(EPWM3_BASE, EPWM_COUNTER_COMPARE_A, cnvAdc2Duty());

}

//========================SPIA=============================

//200k 5us
sPWM pwm200kCfg = { .u32BASE = EPWM4_BASE, .u16TBperiodCount = 499, // 200kHz 的 TBPRD
                    .u16count = 0,              // 初始計數值
                    .u16phaseCount = 0,              // 無相位偏移
                    .u16CompCount = 250 };
//Event-Trigger
void EPWM_TRIGER_ADC(HAL_sPWM p)
{

    //TB
    EPWM_setClockPrescaler(p->u32BASE, EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBasePeriod(p->u32BASE, p->u16TBperiodCount);
    EPWM_setTimeBaseCounter(p->u32BASE, p->u16count);
    EPWM_setTimeBaseCounterMode(p->u32BASE, EPWM_COUNTER_MODE_UP);



    //SOCA to ADC SOC1
    EPWM_enableADCTrigger(p->u32BASE, EPWM_SOC_A);
    EPWM_setADCTriggerSource(p->u32BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_PERIOD);
    EPWM_setADCTriggerEventPrescale(p->u32BASE, EPWM_SOC_A, 1);

}

void init_EPWM(void)
{
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

//    Setup_EPWM(&CLK0);
//    Setup_EPWM(&CLK180);
//    Setup_EPWM(&FANCTRL5);
//
//    // Initialize EPWM4 for ADC triggering (200kHz)
//    // Must be done here to ensure ADC interrupts start immediately
//    EPWM_TRIGER_ADC(&pwm200kCfg);

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}


//  GPIO_togglePin(PIN74_GPIO31_GPIO);
//  GPIO_writePin(PIN74_GPIO31_GPIO,1);
//  GPIO_writePin(PIN74_GPIO31_GPIO,0);

volatile uint32_t *gpioDataReg = (uint32_t *)((uintptr_t)GPIODATA_BASE) +((31 / 32U) * GPIO_DATA_REGS_STEP);
volatile uint32_t *gpioDataReg2 = (uint32_t *)((uintptr_t)GPIODATA_BASE) +((31 / 32U) * GPIO_DATA_REGS_STEP);

ST_SPIA sSPIA = { .REGFIFO = 0,
                  .u16pop = 0,
                  .u16push = 0,
                  .CurFSM = _Idle,
                  .NewFSM = _Mode_Slave

};


 void SPIA_Switch(HAL_SPIA p){

     if (sParams.sDout.o_stat.b18_SPIA_State == 1){
               p->NewFSM = _Mode_Master;
           }else{
               p->NewFSM = _Mode_Slave;
           }

     if(p->NewFSM == p->CurFSM) return;

    switch (p->NewFSM){

    case _Idle:
        break;

    case _Mode_Slave:

        SPIA_ClearInterrupt();
        SPI_disableModule(mySPI0_BASE);
        SPI_reset(mySPI0_BASE);

        RXInterrupt_init();
        //Disable PWM trigger ADC 200K
        // Interrupt_disable(INT_ADCA1);  // ⭐ Commented out to keep ADC ISR running for testing
        EPWM_TRIGER_ADC(&pwm200kCfg);
        Reset_SPI_GPIO();
        #if (SetControlGPIO == Card)
            Common_GPIO();
        #else
            SLAVE_GPIO();
        #endif
        SPIA_Config_S();

        p->CurFSM = p->NewFSM;
        break;

    case _Mode_Master:

        SPIA_ClearInterrupt();
        SPI_disableModule(mySPI0_BASE);
        SPI_reset(mySPI0_BASE);

        TXInterrupt_init();
        //Enable PWM trigger ADC 200K
        Interrupt_enable(INT_ADCA1);
        EPWM_TRIGER_ADC(&pwm200kCfg);
        Reset_SPI_GPIO();
        #if (SetControlGPIO == Card)
            Common_GPIO();
        #else
            MASTER_GPIO();
        #endif
        SPIA_Config_M();

        p->CurFSM = p->NewFSM;
        break;

    default:
        break;
    }
}
 void SPIA_Control(void){
     SPIA_Switch(&sSPIA);
 }

//------------SPI

uint16_t ERROR;

 #ifdef _FLASH
 #pragma SET_CODE_SECTION(".TI.ramfunc")
 #endif //_FLASH

// Slave
__interrupt void INT_mySPI0_RX_ISR(void){
    measTimerLength(&TimerRX);
    GPIO_togglePin(myGPIO0);
    sSPIA.REGFIFO[sSPIA.u16pop].u16AllData = HWREGH(mySPI0_BASE + SPI_O_RXBUF);
//
    uint16_t received_data = sSPIA.REGFIFO[sSPIA.u16pop].Package.u16CV_AD;
    uint16_t received_checksum = sSPIA.REGFIFO[sSPIA.u16pop].Package.u16check;
    uint16_t local_checksum = (received_data + 0x1234) & 0xF;
//
    if (local_checksum == received_checksum){
         DAC_setShadowValue(CC_DA_BASE, received_data);
     }
     else{
         ERROR++;
     }

    GPIO_togglePin(myGPIO0);
     SPI_clearInterruptStatus(mySPI0_BASE, SPI_INT_RX_DATA_TX_EMPTY);
     Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP6);

}
////__interrupt void INT_mySPI0_TX_ISR(void){}



//
//// Dedicated DDS ISR (triggered by EPWM1 - 200kHz)
//// This ISR only handles DDS waveform generation
//__interrupt void INT_DDS_ISR(void) {
//    // DDS Waveform Generation only
//    WaveGen_ISR_Update();
//
//    // Clear interrupt flags
//    EPWM_clearEventTriggerInterruptFlag(CLK0_5_BASE);
//    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
//}

#ifdef _FLASH
#pragma SET_CODE_SECTION()
#endif //_FLASH


//40us update 500ms read //SH
void Temperature (){
    sGbVars.u16TempC_AD        = ADC_readResult(MCP3301_TEMP_RESULT_BASE, ADC_SOC_NUMBER0);
    sParams.sTemp_C.u16Data[0] = sGbVars.u16TempC_AD;
    sParams.sTemp_C.u16Data[1] = 0;
}
