/*
 * spi_master.h
 *
 *  Created on: Apr 1, 2024
 *      Author: User
 */

#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

//#define SPI_MASTER_BASE             SPIB_BASE
//#define SPI_BITRATE                 1000000
#define SPI_DATAWIDTH               16
//
// SPIB_PICO - GPIO Settings
#define SPI_MASTER_SIMO_GPIO        60
#define SPI_MASTER_SIMO_PIN_CONFIG  GPIO_60_SPISIMOB
//
// SPIB_POCI - GPIO Settings
#define SPI_MASTER_SOMI_GPIO        61
#define SPI_MASTER_SOMI_PIN_CONFIG  GPIO_61_SPISOMIB
//
// SPIB_CLK - GPIO Settings
#define SPI_MASTER_CLK_GPIO         58
#define SPI_MASTER_CLK_PIN_CONFIG   GPIO_58_SPICLKB
//
// SPIB_PTE - GPIO Settings
#define SPI_MASTER_CS_GPIO          59
#define SPI_MASTER_CS_PIN_CONFIG    GPIO_59_SPISTEB


#define SCLK_GPIO58_GPIO_PIN_CONFIG GPIO_58_GPIO58
#define SCS_GPIO59_GPIO_PIN_CONFIG GPIO_59_GPIO59
#define SDI_GPIO60_GPIO_PIN_CONFIG GPIO_60_GPIO60
#define SCLK_GPIO58 58
#define SCS_GPIO59 59
#define SDI_GPIO60 60


typedef union {
    uint32_t u32All[2];
    struct {
        uint32_t u32Data:32;
        uint32_t u32Addr:8;
        uint32_t u32Remote:1;  //0: Write, 1:Read
        uint32_t u32Id:7;
        uint32_t u32Chksum:16;
    } bits;

    struct {
        uint32_t W0:16;
        uint32_t W1:16;
        uint32_t W2:16;
        uint32_t W3:16;
    } words;

    uint16_t Dword[4];

    struct {
        uint32_t u32Data:32;
        uint32_t u32Addr:8;
        uint32_t u32Cmd:8;
        uint32_t u32Chksum:16;
    };
}REG_SPIMPACK;

typedef REG_SPIMPACK * HAL_SPIPMACK;


#define SIZE_OF_SPIFIFO      64
#define SPIM_TIMEOUT         T_50US

typedef enum {
    _SEND_SPI_PACK       = (0x00000001<<0),
    _RECEIVE_SPI_PACK    = (0x00000001<<1),
    _CLEAR_SPI_PACK      = (0x00000001<<4),
    _MARK_ERROR_SPI_M_PACK = (0x80000000)
}FSM_SPI_MASTER;

typedef struct {
    FSM_SPI_MASTER u32Fsm;
    REG_SPIMPACK regFiFo[SIZE_OF_SPIFIFO];
    uint16_t u16Push;
    uint16_t u16Pop;
    uint32_t u32Timeout;
    uint32_t u32TimeStamp;
    uint32_t u32TimeMark;
    REG_SPIMPACK rxTemp;
    uint32_t u32ErrCnts;
    uint32_t u32OkCnts;
    int16_t s16Wsize;
    int16_t s16Rsize;
    int16_t s16Index;

}ST_SPI_MASTER;

typedef ST_SPI_MASTER * HAL_SPI_MASTER;


typedef enum {
    _NO_M_SPI_COMMMAND = 0,
    _CMD_M_RESERVED0 = (0x01<<0),
    _CMD_M_RESERVED1 = (0x01<<1),
    _CMD_M_RESERVED2 = (0x01<<2),
    _CMD_M_ACCESS_MEMORY  = (0x01<<3),
    _CMD_M_WRITE_MEMORY       = _CMD_M_ACCESS_MEMORY + 1,
    _CMD_M_READ_MEMORY_HEADER = _CMD_M_ACCESS_MEMORY + 2,
    _CMD_M_READ_MEMORY_DATA   = _CMD_M_ACCESS_MEMORY + 3,
    _CMD_M_ACCESS_32BITS  = (0x01<<4),
    _CMD_M_WRITE_32BITS     = _CMD_M_ACCESS_32BITS + 1,
    _CMD_M_SEND_READ_HEADER = _CMD_M_ACCESS_32BITS + 2,
    _CMD_M_GET_READ_32BITS  = _CMD_M_ACCESS_32BITS + 3,
    _CMD_M_RESERVED5 = (0x01<<5),
    _CMD_M_RESERVED6 = (0x01<<6)
}REG_M_SPICMD;

static inline void wrSpiMData(uint16_t u16Data)
{
    SPI_writeDataBlockingFIFO(mySPI0_BASE, u16Data);
}

static inline uint16_t rdSpiMData(void)
{
    return SPI_readDataNonBlocking(mySPI0_BASE);
}

static inline uint16_t isRxMEmpty(void)
{
    return (SPI_FIFO_RXEMPTY != SPI_getRxFIFOStatus(mySPI0_BASE));
}

static inline int16_t isSpiPackFiFoFree(HAL_SPI_MASTER v){

    return (v->u16Push == v->u16Pop) ;
}


static inline void pushSpiWpackForID(uint32_t addr, uint32_t *pdata, REG_M_SPICMD id, HAL_SPI_MASTER v)
{
    HAL_SPIPMACK p = &v->regFiFo[v->u16Push];
    p->bits.u32Id = id;
    p->bits.u32Remote = 0;
    p->bits.u32Addr = addr;
    p->bits.u32Data = pdata[addr];
    p->bits.u32Chksum = p->words.W0 + p->words.W1 + p->words.W2;
    v->u16Push++;
    if(SIZE_OF_SPIFIFO == v->u16Push) v->u16Push = 0;
}

static inline void pushSpiRheaderForID(uint32_t addr, uint32_t *pdata, REG_M_SPICMD id, HAL_SPI_MASTER v)
{
    HAL_SPIPMACK p = &v->regFiFo[v->u16Push];
    p->bits.u32Id = id;
    p->bits.u32Remote = 1;
    p->bits.u32Addr = addr;
    p->bits.u32Data = (uint32_t) &pdata[addr];
    p->bits.u32Chksum = p->words.W2;
    v->u16Push++;
    if(SIZE_OF_SPIFIFO == v->u16Push) v->u16Push = 0;
}

static inline void pushSpiRdataForID(uint32_t addr, uint32_t *pdata, REG_M_SPICMD id, HAL_SPI_MASTER v)
{
    HAL_SPIPMACK p = &v->regFiFo[v->u16Push];
    p->bits.u32Id = id;
    p->bits.u32Remote = 1;
    p->bits.u32Addr = addr;
    p->bits.u32Data = (uint32_t) &pdata[addr];
    p->bits.u32Chksum = p->words.W0 + p->words.W1 + p->words.W2;
    v->u16Push++;
    if(SIZE_OF_SPIFIFO == v->u16Push) v->u16Push = 0;
}

static inline void pushSpiWpack(uint32_t addr, uint32_t *pdata, HAL_SPI_MASTER v)
{
    pushSpiWpackForID(addr, pdata, _CMD_M_WRITE_32BITS, v);
}

static inline void pushSpiRheader(uint32_t addr, uint32_t *pdata, HAL_SPI_MASTER v)
{
    pushSpiRheaderForID(addr, pdata, _CMD_M_SEND_READ_HEADER, v);
}

static inline void pushSpiRdata(uint32_t addr, uint32_t *pdata, HAL_SPI_MASTER v)
{
    pushSpiRheaderForID(addr, pdata, _CMD_M_GET_READ_32BITS, v);
}

static inline void pushSpiRpack(uint32_t addr, uint32_t *pdata, HAL_SPI_MASTER v)
{
    pushSpiRheader(addr, pdata, v);
    pushSpiRdata(addr, pdata, v);
}


static inline void pushSpiWriteAddress(uint32_t addr, uint32_t *pdata, HAL_SPI_MASTER v)
{
    pushSpiWpackForID(addr, pdata, _CMD_M_WRITE_MEMORY, v);
}

static inline void pushSpiReadHeader(uint32_t addr, uint32_t *pdata, HAL_SPI_MASTER v)
{
    pushSpiRheaderForID(addr, pdata, _CMD_M_READ_MEMORY_HEADER, v);
}

static inline void pushSpiReadData(uint32_t addr, uint32_t *pdata, HAL_SPI_MASTER v)
{
    pushSpiRheaderForID(addr, pdata, _CMD_M_READ_MEMORY_DATA, v);
}

static inline void pushSpiReadAddress(uint32_t addr, uint32_t *pdata, HAL_SPI_MASTER v)
{
    pushSpiReadHeader(addr, pdata, v);
    pushSpiReadData(addr, pdata, v);
}

static inline int16_t wrSpiMPack(HAL_SPIPMACK p, HAL_SPI_MASTER v)
{
    switch(p->bits.u32Id) {
    case _CMD_M_WRITE_32BITS:
    case _CMD_M_WRITE_MEMORY:
        wrSpiMData(p->words.W3);
        wrSpiMData(p->words.W2);
        wrSpiMData(p->words.W1);
        wrSpiMData(p->words.W0);
        v->s16Rsize = 0;
        return 4;

    case _CMD_M_GET_READ_32BITS:
    case _CMD_M_READ_MEMORY_DATA:
        wrSpiMData(p->words.W3);
        wrSpiMData(p->words.W2);
        wrSpiMData(p->words.W1);
        wrSpiMData(p->words.W0);
        v->s16Rsize = 4;
        return 4;
    case _CMD_M_SEND_READ_HEADER:
    case _CMD_M_READ_MEMORY_HEADER:
        wrSpiMData(p->words.W3);
        wrSpiMData(p->words.W2);
        v->s16Rsize = 0;
        return 2;

    default:
        return 0;
    }
}

static inline void popSpiWdata(HAL_SPI_MASTER v)
{
    if(v->u16Pop != v->u16Push) {

        switch(v->u32Fsm) {
        case _SEND_SPI_PACK:
            if(0 == v->u32TimeMark) {
                v->s16Wsize = wrSpiMPack((HAL_SPIPMACK) &v->regFiFo[v->u16Pop], v);
                if(0 < v->s16Wsize) {
                    v->u32TimeMark = v->u32TimeStamp = U32_UPCNTS;
                    v->s16Index = 0;
                    if(0<v->s16Rsize) {
                        v->u32Fsm = _RECEIVE_SPI_PACK;
                    }
                    else {
                        v->u32Fsm = _CLEAR_SPI_PACK;
                    }
                }
                else {
                    v->u16Pop++;
                    v->u32ErrCnts++;
                    v->u32Fsm |= _MARK_ERROR_SPI_M_PACK;
                }
            }
            break;

        case _CLEAR_SPI_PACK:
            if(0 < v->s16Wsize) {
                while(isRxMEmpty()) {
                   rdSpiMData();
                   v->u32TimeMark = v->u32TimeStamp = U32_UPCNTS;
                   v->s16Wsize--;
                   if(0 == v->s16Wsize) break;
                }
            }
            else {
                v->rxTemp = v->regFiFo[v->u16Pop]; //Make a copy
                v->u32OkCnts++; //Count the number of success
                v->u16Pop++;
                if(SIZE_OF_SPIFIFO == v->u16Pop) v->u16Pop = 0;
                //v->u32TimeMark = 0;
                v->u32Fsm = _SEND_SPI_PACK;
            }
            break;

        case _RECEIVE_SPI_PACK:
            if(v->s16Index < v->s16Rsize) {
                while(isRxMEmpty()) {
                    v->rxTemp.Dword[v->s16Index++] = rdSpiMData();
                    v->u32TimeMark = v->u32TimeStamp = U32_UPCNTS;
                    if(v->s16Index == v->s16Rsize) break;
                }
            }
            else {
                v->rxTemp.words.W3 -= (v->rxTemp.words.W2
                                      +v->rxTemp.words.W1
                                      +v->rxTemp.words.W0);

                if((0 == v->rxTemp.bits.u32Chksum)&&(0 != v->rxTemp.words.W2)) {
                    *((uint32_t *)v->regFiFo[v->u16Pop].bits.u32Data) = v->rxTemp.bits.u32Data;
                    v->u32OkCnts++;
                }
                else {
                    v->u32ErrCnts++;
                }

                v->u16Pop++;
                if(SIZE_OF_SPIFIFO == v->u16Pop) v->u16Pop = 0;
                //v->u32TimeMark = 0;
                v->u32Fsm = _SEND_SPI_PACK;
            }
            break;

        default:
            break;
        }

        if(0 != v->u32TimeMark) {
            v->u32TimeStamp = U32_UPCNTS;

            if(v->u32TimeMark > v->u32TimeStamp) {
                v->u32Timeout = v->u32TimeStamp + SW_TIMER - v->u32TimeMark;
            }
            else {
                v->u32Timeout = v->u32TimeStamp - v->u32TimeMark;
            }

            if(v->u32Timeout >= SPIM_TIMEOUT) {
                v->u32TimeMark = 0;
                v->s16Rsize = v->s16Wsize = 0;

            }
        }
    }
}

//static inline void initSPIxMasterGpio()
//{
//    GPIO_setPinConfig(SPI_MASTER_SIMO_PIN_CONFIG);
//    GPIO_setPadConfig(SPI_MASTER_SIMO_GPIO, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(SPI_MASTER_SIMO_GPIO, GPIO_QUAL_ASYNC);
//
//    GPIO_setPinConfig(SPI_MASTER_SOMI_PIN_CONFIG);
//    GPIO_setPadConfig(SPI_MASTER_SOMI_GPIO, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(SPI_MASTER_SOMI_GPIO, GPIO_QUAL_ASYNC);
//
//    GPIO_setPinConfig(SPI_MASTER_CLK_PIN_CONFIG);
//    GPIO_setPadConfig(SPI_MASTER_CLK_GPIO, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(SPI_MASTER_CLK_GPIO, GPIO_QUAL_ASYNC);
//
//    GPIO_setPinConfig(SPI_MASTER_CS_PIN_CONFIG);
//    GPIO_setPadConfig(SPI_MASTER_CS_GPIO, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(SPI_MASTER_CS_GPIO, GPIO_QUAL_ASYNC);
//
//}
//
//static inline void initSPIxMaster(){
//    SPI_disableModule(SPI_MASTER_BASE);
//    SPI_setConfig(SPI_MASTER_BASE, DEVICE_LSPCLK_FREQ, SPI_PROT_POL0PHA0,
//                  SPI_MODE_CONTROLLER, SPI_BITRATE, SPI_DATAWIDTH);
//    SPI_setPTESignalPolarity(SPI_MASTER_BASE, SPI_PTE_ACTIVE_LOW);
//    SPI_enableFIFO(SPI_MASTER_BASE);
//    SPI_disableLoopback(SPI_MASTER_BASE);
//    SPI_setEmulationMode(SPI_MASTER_BASE, SPI_EMULATION_FREE_RUN);
//    SPI_enableModule(SPI_MASTER_BASE);
//}
//
//static inline void setSPIx2GpioMode()
//{
//    // GPIO58 -> SCLK_GPIO58 Pinmux
//    GPIO_setPinConfig(GPIO_58_GPIO58);
//    // GPIO59 -> SCS_GPIO59 Pinmux
//    GPIO_setPinConfig(GPIO_59_GPIO59);
//    // GPIO60 -> SDI_GPIO60 Pinmux
//    GPIO_setPinConfig(GPIO_60_GPIO60);
//
//
//    GPIO_writePin(SCLK_GPIO58, 1);
//    GPIO_setPadConfig(SCLK_GPIO58, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(SCLK_GPIO58, GPIO_QUAL_SYNC);
//    GPIO_setDirectionMode(SCLK_GPIO58, GPIO_DIR_MODE_OUT);
//    GPIO_setControllerCore(SCLK_GPIO58, GPIO_CORE_CPU1);
//
//    GPIO_writePin(SCS_GPIO59, 1);
//    GPIO_setPadConfig(SCS_GPIO59, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(SCS_GPIO59, GPIO_QUAL_SYNC);
//    GPIO_setDirectionMode(SCS_GPIO59, GPIO_DIR_MODE_OUT);
//    GPIO_setControllerCore(SCS_GPIO59, GPIO_CORE_CPU1);
//
//    GPIO_writePin(SDI_GPIO60, 1);
//    GPIO_setPadConfig(SDI_GPIO60, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(SDI_GPIO60, GPIO_QUAL_SYNC);
//    GPIO_setDirectionMode(SDI_GPIO60, GPIO_DIR_MODE_OUT);
//    GPIO_setControllerCore(SDI_GPIO60, GPIO_CORE_CPU1);
//
//}

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
        uint32_t b18_31_reserved:14;
    } i_stat;

    struct {
        uint32_t b0_15_reserved:16;
        uint32_t b16_off_pwm_n:1;
        uint32_t b17_master:1;
        uint32_t b18_31_reserved:14;
    } o_stat;
} REG_MPARAM;

typedef enum {
    _NO_ACTION_FOR_ISTAT =  0x00000000,
    _ISTAT_PFC_OVP       = (0x00000001<<0),
    _ISTAT_PFC_UVP       = (0x00000001<<1),
    _ISTAT_PFC2_OVP      = (0x00000001<<2),
    _ISTAT_PFC2_UVP      = (0x00000001<<3),
    _ISTAT_PFC_OTP       = (0x00000001<<4),
    _MASK_PFC_ERROR      = (_ISTAT_PFC_OVP|_ISTAT_PFC_UVP|_ISTAT_PFC2_OVP|_ISTAT_PFC2_UVP|_ISTAT_PFC_OTP),
    _ISTAT_DC_OVP        = (0x00000001<<5),
    _ISTAT_AC_FAIL       = (0x00000001<<6),
    _ISTAT_FAN1_FAIL     = (0x00000001<<7),
    _ISTAT_FAN2_FAIL     = (0x00000001<<8),
    _ISTAT_FAN3_FAIL     = (0x00000001<<9),
    _ISTAT_FAN4_FAIL     = (0x00000001<<10),
    _MASK_FAN_HALF_FAIL  = (_ISTAT_FAN1_FAIL|_ISTAT_FAN2_FAIL),
    _MASK_FAN_FULL_FAIL  = (_ISTAT_FAN1_FAIL|_ISTAT_FAN2_FAIL|_ISTAT_FAN3_FAIL|_ISTAT_FAN4_FAIL)

}REG_ISTAT;

// Define a struct containing three register parameters
typedef struct {
    REG_MPARAM sDin;
    REG_MPARAM sDout;
    REG_MPARAM sFanCtrl;
    REG_MPARAM sIref_P;
    REG_MPARAM sIref_N;
    REG_MPARAM sTemp_C;
    REG_MPARAM sHeartbeats;
} ST_MPARAMS;

// Macro to calculate the offset of a member in the struct
#define INIT_MPARAM(name) (uintptr_t)(REG_MPARAM*)(&((ST_MPARAMS*)(0))->name)/sizeof(REG_MPARAM)

// Define enum with calculated offsets
typedef enum {
    _IDM_DIN        = INIT_MPARAM(sDin),
    _IDM_DOUT       = INIT_MPARAM(sDout),
    _IDM_FAN_CTRL   = INIT_MPARAM(sFanCtrl),
    _IDM_IREF_P     = INIT_MPARAM(sIref_P),
    _IDM_IREF_N     = INIT_MPARAM(sIref_N),
    _IDM_TEMP_C     = INIT_MPARAM(sTemp_C),
    _IDM_HEARTBEAT  = INIT_MPARAM(sHeartbeats),
    _END_OF_IDMCMD
} IDM_CMD;

extern ST_MPARAMS sMParams;
extern void scanSpiAction(void);
extern void popSpiPack(void);

#endif /* SPI_MASTER_H_ */
