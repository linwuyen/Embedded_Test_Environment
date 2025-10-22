/*
 * initCLK.h
 *
 *  Created on: May 27, 2024
 *      Author: roger_lin
 */

#ifndef SPWM_CTRL_H_
#define SPWM_CTRL_H_

//==========SPIA=========
#define mySPI0_BASE      SPIA_BASE
#define mySPI0_BITRATE   5000000

// Interrupt Settings for INT_mySPI0_RX
#define INT_mySPI0_RX INT_SPIA_RX
#define INT_mySPI0_RX_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP6
extern __interrupt void INT_mySPI0_RX_ISR(void);

// Interrupt Settings for INT_mySPI0_TX
//#define INT_mySPI0_TX INT_SPIA_TX
//#define INT_mySPI0_TX_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP6
//extern __interrupt void INT_mySPI0_TX_ISR(void);

//========SPI_Common_Test_PIN===========

#define Card                          0
#define ASR                           1
#define SetControlGPIO                ASR

#define Common_PICO                   16
#define Common_PICO_GPIO              16
#define Common_PICO_PIN_CONFIG        GPIO_16_SPIA_SIMO
//

#define Common_POCI                   17
#define Common_POCI_GPIO              17
#define Common_POCI_PIN_CONFIG        GPIO_17_SPIA_SOMI


// SPIA_CLK - GPIO Settings
//
#define Common_CLK                     9
#define Common_SPICLK_GPIO             9
#define Common_SPICLK_PIN_CONFIG       GPIO_9_SPIA_CLK
//
// SPIA_PTE - GPIO Settings

#define Common_PTE                     11
#define Common_SPIPTE_GPIO             11
#define Common_SPIPTE_PIN_CONFIG       GPIO_11_SPIA_STE

//=====SPIA_MASTER PIN==================
//
#define GPIO_PIN_SPIA_PICO             8//16
#define mySPI0_SPIPICO_GPIO            8//16
#define mySPI0_SPIPICO_PIN_CONFIG      GPIO_8_SPIA_SIMO//GPIO_16_SPIA_SIMO
//
// SPIA_CLK - GPIO Settings
//
#define GPIO_PIN_SPIA_CLK              3//9
#define mySPI0_SPICLK_GPIO             3//9
#define mySPI0_SPICLK_PIN_CONFIG       GPIO_3_SPIA_CLK//GPIO_9_SPIA_CLK
//
// SPIA_PTE - GPIO Settings

#define GPIO_PIN_SPIA_PTE              11//5
#define mySPI0_SPIPTE_GPIO             11//5
#define mySPI0_SPIPTE_PIN_CONFIG       GPIO_11_SPIA_STE//GPIO_5_SPIA_STE


//=====SPIA_SLAVE PIN==============

#define GPIO_PIN_SPIA_PICO_S          16///8
#define mySPI0_SPIPICO_GPIO_S         16//8
#define mySPI0_SPIPICO_PIN_CONFIG_S   GPIO_16_SPIA_SIMO//GPIO_8_SPIA_SIMO
//
// SPIA_CLK - GPIO Settings
//
#define GPIO_PIN_SPIA_CLK_S           9//3
#define mySPI0_SPICLK_GPIO_S          9//3
#define mySPI0_SPICLK_PIN_CONFIG_S    GPIO_9_SPIA_CLK//GPIO_3_SPIA_CLK
//
// SPIA_PTE - GPIO Settings
//
#define GPIO_PIN_SPIA_PTE_S           5//11
#define mySPI0_SPIPTE_GPIO_S          5//11
#define mySPI0_SPIPTE_PIN_CONFIG_S    GPIO_5_SPIA_STE//GPIO_11_SPIA_STE

//---------------------------------

#define BUFFER_SIZE 64
//#define FIFO_SIZE 2
#define FIFO_SIZE 3
#define SPI_CalcChecksum(data)    (((((data) >> 8) & 0xFF) ^ ((data) & 0xFF)) & 0x0F)

typedef enum{
    _Idle                  = (0x00000001<<0),
    _Mode_Slave            = (0x00000001<<1),
    _Mode_Master           = (0x00000001<<2),

    _MARK_ERROR_SPI_C_PACK = (0x80000000)
}FSM_SPIA;


typedef union{
        uint16_t u16AllData;
    struct{
        uint16_t u16check:4;
        uint16_t u16CV_AD:12;
    }Package;
}SPI_PACK;


typedef struct{
    FSM_SPIA    NewFSM;
    FSM_SPIA    CurFSM;
    SPI_PACK    REGFIFO[FIFO_SIZE];
    uint16_t    u16pop;
    uint16_t    u16push;
    uint16_t    u16pull_slave;
    uint16_t   u16pull_dac;
    uint16_t    u16ChkSuccess;
    uint16_t    u16ChkFail;

    uint16_t CHECKSUM1;
    uint16_t CHECKSUM2;
    uint16_t CHECKSUM3;
}ST_SPIA;

typedef ST_SPIA * HAL_SPIA;



//for Control Card
static inline void Common_GPIO(void){

    GPIO_setPinConfig(Common_PICO_PIN_CONFIG);
    GPIO_setPadConfig(Common_PICO_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(Common_PICO_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(Common_SPICLK_PIN_CONFIG);
    GPIO_setPadConfig(Common_SPICLK_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(Common_SPICLK_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(Common_SPIPTE_PIN_CONFIG);
    GPIO_setPadConfig(Common_SPIPTE_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(Common_SPIPTE_GPIO, GPIO_QUAL_ASYNC);
}

static inline void MASTER_GPIO(void){

    GPIO_setPinConfig(mySPI0_SPIPICO_PIN_CONFIG);
    GPIO_setPadConfig(mySPI0_SPIPICO_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(mySPI0_SPIPICO_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(mySPI0_SPICLK_PIN_CONFIG);
    GPIO_setPadConfig(mySPI0_SPICLK_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(mySPI0_SPICLK_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(mySPI0_SPIPTE_PIN_CONFIG);
    GPIO_setPadConfig(mySPI0_SPIPTE_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(mySPI0_SPIPTE_GPIO, GPIO_QUAL_ASYNC);
}

static inline void SLAVE_GPIO(void){

    GPIO_setPinConfig(mySPI0_SPIPICO_PIN_CONFIG_S);
    GPIO_setPadConfig(mySPI0_SPIPICO_GPIO_S, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(mySPI0_SPIPICO_GPIO_S, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(mySPI0_SPICLK_PIN_CONFIG_S);
    GPIO_setPadConfig(mySPI0_SPICLK_GPIO_S, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(mySPI0_SPICLK_GPIO_S, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(mySPI0_SPIPTE_PIN_CONFIG_S);
    GPIO_setPadConfig(mySPI0_SPIPTE_GPIO_S, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(mySPI0_SPIPTE_GPIO_S, GPIO_QUAL_ASYNC);
}

static inline void Reset_SPI_GPIO(void){

//    GPIO_setPinConfig


    GPIO_setPinConfig(GPIO_8_GPIO8);
//    GPIO_setDirectionMode(GPIO_8_GPIO8,GPIO_DIR_MODE_IN);

    GPIO_setPinConfig(GPIO_3_GPIO3);
//    GPIO_setDirectionMode(GPIO_3_GPIO3,GPIO_DIR_MODE_IN);

    GPIO_setPinConfig(GPIO_11_GPIO11);
//    GPIO_setDirectionMode(GPIO_11_GPIO11,GPIO_DIR_MODE_IN);

    GPIO_setPinConfig(GPIO_16_GPIO16);
//    GPIO_setDirectionMode(GPIO_16_GPIO16,GPIO_DIR_MODE_IN);

    GPIO_setPinConfig(GPIO_9_GPIO9);
//    GPIO_setDirectionMode(GPIO_9_GPIO9,GPIO_DIR_MODE_IN);

    GPIO_setPinConfig(GPIO_5_GPIO5);
//    GPIO_setDirectionMode(GPIO_5_GPIO5,GPIO_DIR_MODE_IN);
}

//Master
static inline void SPIA_Config_M(){

    SPI_disableModule(mySPI0_BASE);
    SPI_setConfig(mySPI0_BASE, DEVICE_LSPCLK_FREQ, SPI_PROT_POL0PHA0,SPI_MODE_CONTROLLER, 5000000, 16);

    SPI_setPTESignalPolarity(mySPI0_BASE, SPI_PTE_ACTIVE_LOW);
    SPI_disableLoopback(mySPI0_BASE);
    SPI_setEmulationMode(mySPI0_BASE, SPI_EMULATION_FREE_RUN);
    SPI_enableModule(mySPI0_BASE);
}


//Slave
static inline void SPIA_Config_S(){

//    SPI_disableModule(mySPI0_BASE);
//    SPI_setConfig(mySPI0_BASE, DEVICE_LSPCLK_FREQ, SPI_PROT_POL0PHA0, SPI_MODE_PERIPHERAL, 5000000, 16);
//    SPI_setPTESignalPolarity(mySPI0_BASE, SPI_PTE_ACTIVE_LOW);
////    SPI_enableFIFO(mySPI0_BASE);
////    SPI_setFIFOInterruptLevel(mySPI0_BASE, SPI_FIFO_TXEMPTY, SPI_FIFO_RX1);
//    SPI_setFIFOInterruptLevel(mySPI0_BASE, SPI_FIFO_TXEMPTY, SPI_FIFO_RXEMPTY);
//    SPI_disableLoopback(mySPI0_BASE);
//    SPI_setEmulationMode(mySPI0_BASE, SPI_EMULATION_STOP_MIDWAY);
//    SPI_enableModule(mySPI0_BASE);
//    SPI_enableInterrupt(mySPI0_BASE, SPI_INT_RXFF);

    SPI_disableModule(mySPI0_BASE);
    SPI_setConfig(mySPI0_BASE, DEVICE_LSPCLK_FREQ, SPI_PROT_POL0PHA0,SPI_MODE_PERIPHERAL, 5000000, 16);
    SPI_setPTESignalPolarity(mySPI0_BASE, SPI_PTE_ACTIVE_LOW);
    SPI_disableFIFO(mySPI0_BASE);
    SPI_clearInterruptStatus(mySPI0_BASE, SPI_INT_RX_DATA_TX_EMPTY);
    SPI_enableInterrupt(mySPI0_BASE, SPI_INT_RX_DATA_TX_EMPTY);
    SPI_disableLoopback(mySPI0_BASE);
    SPI_setEmulationMode(mySPI0_BASE, SPI_EMULATION_FREE_RUN);
    SPI_enableModule(mySPI0_BASE);
}

static inline void RXInterrupt_init(void){
// Interrupt Setings for INT_mySPI0_RX
Interrupt_register(INT_mySPI0_RX, &INT_mySPI0_RX_ISR);
Interrupt_enable(INT_mySPI0_RX);
}

static inline void TXInterrupt_init(void){

    // Interrupt Setings for INT_CV_AD_1
    Interrupt_register(INT_CV_AD_1, &INT_CV_AD_1_ISR);
    Interrupt_enable(INT_CV_AD_1);


    // Interrupt Setings for INT_mySPI0_TX
//    Interrupt_register(INT_mySPI0_TX, &INT_mySPI0_TX_ISR);
//    Interrupt_disable(INT_mySPI0_TX);

}

static inline uint16_t calcsum(uint16_t data) {

    uint16_t high, low, checksum;

    high = ( data >> 8) & 0x00FF;  // 高 8 bit
    low  =  data & 0x00FF;         // 低 8 bit

    checksum = (high ^ low) & 0x000F;    // 只保留 4 bit

    return checksum;
}

static inline  void SPIA_ClearInterrupt(void){

    SPI_disableInterrupt(mySPI0_BASE, SPI_INT_RXFF);
    SPI_clearInterruptStatus(mySPI0_BASE, SPI_INT_RXFF);
    SPI_disableInterrupt(mySPI0_BASE, SPI_INT_RX_DATA_TX_EMPTY);
    SPI_clearInterruptStatus(mySPI0_BASE, SPI_INT_RX_DATA_TX_EMPTY);

}

extern  void SPIA_Control(void);
///=============================================================================================
typedef union {
    float32_t f32Data;
    uint32_t u32Data;
    uint16_t u16Data[2];

    struct {
        uint32_t b0_pfc_ovp_n:1;
        uint32_t b1_pfc_uvp_n:1;
        uint32_t b2_pfc_ovp_2_n:1;
        uint32_t b3_pfc_uvp_2_n:1;
        uint32_t b4_pfc_otp_n:1;
        uint32_t b5_dc_ovp_n:1;
        uint32_t b6_ac_fail_5_n:1;
        uint32_t b7_fan_fail_1:1;
        uint32_t b8_fan_fail_2:1;
        uint32_t b9_fan_fail_3:1;
        uint32_t b10_fan_fail_4:1;
        uint32_t b11_15_reserved:5;
        uint32_t b16_off_pwm_n:1;
        uint32_t b17_master:1;
        uint32_t b18_SPIA_State:1;
        uint32_t b19_31_reserved:14;
    } i_stat;

    struct {
        uint32_t b0_15_reserved:16;
        uint32_t b16_off_pwm_n:1;
        uint32_t b17_master:1;
        uint32_t b18_SPIA_State:1;
        uint32_t b19_31_reserved:14;
    } o_stat;
} REG_PARAM;



// Define a struct containing three register parameters
typedef struct {
    REG_PARAM sDin;
    REG_PARAM sDout;
    REG_PARAM sFanCtrl;
    REG_PARAM sIref_P;
    REG_PARAM sIref_N;
    REG_PARAM sTemp_C;
    REG_PARAM sHeartbeats;
} ST_PARAMS;

// Macro to calculate the offset of a member in the struct
#define INIT_PARAM(name) (uintptr_t)(REG_PARAM*)(&((ST_PARAMS*)(0))->name)/sizeof(REG_PARAM)

// Define enum with calculated offsets
typedef enum {
    _ID_DIN      = INIT_PARAM(sDin),
    _ID_DOUT     = INIT_PARAM(sDout),
    _ID_FAN_CTRL = INIT_PARAM(sFanCtrl),
    _ID_IREF_P   = INIT_PARAM(sIref_P),
    _ID_IREF_N   = INIT_PARAM(sIref_N),
    _ID_TEMP_C   = INIT_PARAM(sTemp_C),
    _END_OF_IDCMD
} ID_CMD;

extern  ST_PARAMS sParams;

//timetask.c- task10msec
extern void Update_IO(void);
extern void Temperature();
extern void init_EPWM(void);

extern void updateFANCTRL5Duty( void);

#endif /* SPWM_CTRL_H_ */
