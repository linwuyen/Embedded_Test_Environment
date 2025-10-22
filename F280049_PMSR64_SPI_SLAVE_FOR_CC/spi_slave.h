/*
 * spi_slave.h
 *
 *  Created on: Apr 1, 2024
 *      Author: cody_chen
 */

#ifndef SPI_SLAVE_H_
#define SPI_SLAVE_H_

//power up KEEP
#define SPI_SLAVE_BASE SPIB_BASE

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
}REG_SPIPACK;

typedef REG_SPIPACK * HAL_SPIPACK;


#define SIZE_OF_SPIFIFO      64
#define SPI_TIMEOUT          T_10US   //Must be less than SPI_MASTER


typedef enum {
    _IS_THRER_A_SPI_PACK = (0x00000001<<0),
    _PARSE_THE_SPI_PACK  = (0x00000001<<1),
    _CLEAR_SPI_TXRX_PACK = (0x00000001<<2),
    _MARK_ERROR_SPI_PACK = (0x80000000)
}FSM_SPI_SLAVE;

typedef struct {
    FSM_SPI_SLAVE u32Fsm;
    REG_SPIPACK regFiFo[SIZE_OF_SPIFIFO];
    uint16_t u16Push;
    uint16_t u16Pop;
    uint32_t u32Timeout;
    uint32_t u32TimeStamp;
    uint32_t u32TimeMark;
    REG_SPIPACK rxTemp;
    uint32_t u32ErrCnts;
    uint32_t u32OkCnts;
    int16_t s16Wsize;
    int16_t s16Rsize;
    uint32_t *p32Data;
    uint32_t *p32Eeprom;
    uint32_t u32CntWriteEeprom;

}ST_SPI_SLAVE;

typedef ST_SPI_SLAVE * HAL_SPI_SLAVE;


typedef enum {
    _NO_SPI_COMMMAND = 0,
    _CMD_RESERVED0 = (0x01<<0),
    _CMD_RESERVED1 = (0x01<<1),
    _CMD_RESERVED2 = (0x01<<2),
    _CMD_ACCESS_MEMORY  = (0x01<<3),
    _CMD_WRITE_MEMORY       = _CMD_ACCESS_MEMORY+1,
    _CMD_READ_MEMORY_HEADER = _CMD_ACCESS_MEMORY+2,
    _CMD_READ_MEMORY_DATA   = _CMD_ACCESS_MEMORY+3,
    _CMD_ACCESS_32BITS      = (0x01<<4),
    _CMD_WRITE_32BITS       = _CMD_ACCESS_32BITS+1,
    _CMD_GET_READ_HEADER    = _CMD_ACCESS_32BITS+2,
    _CMD_SEND_READ_32BITS   = _CMD_ACCESS_32BITS+3,
    _CMD_RESERVED5 = (0x01<<5),
    _CMD_RESERVED6 = (0x01<<6)
}REG_SPICMD;

static inline void wrSpiData(uint16_t u16Data)
{
    SPI_writeDataBlockingFIFO(SPI_SLAVE_BASE, u16Data);
}

static inline uint16_t rdSpiData(void)
{
    return SPI_readDataNonBlocking(SPI_SLAVE_BASE);
}

static inline uint16_t isRxEmpty(void)
{
    return (SPI_FIFO_RXEMPTY != SPI_getRxFIFOStatus(SPI_SLAVE_BASE));
}

static inline void rstRxFiFoEmpty(void)
{
    SPI_resetRxFIFO(SPI_SLAVE_BASE);
}

static inline void rstTxFiFoEmpty(void)
{
    SPI_resetTxFIFO(SPI_SLAVE_BASE);
}


static inline int16_t wrSpiPack(HAL_SPIPACK p)
{
    wrSpiData(p->words.W0);
    wrSpiData(p->words.W1);
    wrSpiData(p->words.W2);
    wrSpiData(p->words.W3);
    return 4;
}

static inline void excSpiSlave(HAL_SPI_SLAVE v)
{
    switch(v->u32Fsm) {
    case _IS_THRER_A_SPI_PACK:
        while(isRxEmpty()) {
            if(0 == v->u32TimeMark) {
                v->s16Rsize = 4;
                v->u32TimeMark = v->u32TimeStamp = U32_UPCNTS;
            }
            else if(0<v->s16Rsize){
                v->rxTemp.Dword[v->s16Rsize-1] = rdSpiData();
                v->u32TimeMark = v->u32TimeStamp = U32_UPCNTS;
                v->s16Rsize--;
            }
            else {
                // Out of range of v->s16Rsize
                break;
            }
        }

        if(0 != v->u32TimeMark) {
            v->u32TimeStamp = U32_UPCNTS;

            if(v->u32TimeMark > v->u32TimeStamp) {
                v->u32Timeout = v->u32TimeStamp + SW_TIMER - v->u32TimeMark;
            }
            else {
                v->u32Timeout = v->u32TimeStamp - v->u32TimeMark;
            }

            if(v->u32Timeout >= SPI_TIMEOUT) {
                v->u32TimeMark = 0;
                rstTxFiFoEmpty();
                v->u32Fsm = _PARSE_THE_SPI_PACK;
            }
        }

        break;

    case _PARSE_THE_SPI_PACK:
        switch(v->rxTemp.bits.u32Id) {
        case _CMD_WRITE_32BITS:
            v->rxTemp.bits.u32Chksum -= (v->rxTemp.words.W0 +v->rxTemp.words.W1 + v->rxTemp.words.W2);
            if(0 == v->rxTemp.bits.u32Chksum) {
                v->p32Data[v->rxTemp.bits.u32Addr] = v->rxTemp.bits.u32Data;
                v->u32OkCnts++;
            }
            else {
                v->u32ErrCnts++;
            }
            break;
        case _CMD_GET_READ_HEADER:
            v->rxTemp.bits.u32Data = v->p32Data[v->rxTemp.bits.u32Addr];
            v->rxTemp.bits.u32Chksum = v->rxTemp.words.W0 +v->rxTemp.words.W1 + v->rxTemp.words.W2;
            wrSpiPack(&v->rxTemp);
            v->u32OkCnts++;
            break;
        case _CMD_WRITE_MEMORY:
            v->rxTemp.bits.u32Chksum -= (v->rxTemp.words.W0 +v->rxTemp.words.W1 + v->rxTemp.words.W2);
            if(0 == v->rxTemp.bits.u32Chksum) {
                v->p32Eeprom[v->rxTemp.bits.u32Addr] = v->rxTemp.bits.u32Data;
                v->u32CntWriteEeprom++;
                v->u32OkCnts++;
            }
            else {
                v->u32ErrCnts++;
            }
            break;
        case _CMD_READ_MEMORY_HEADER:
            v->rxTemp.bits.u32Data = v->p32Eeprom[v->rxTemp.bits.u32Addr];
            v->rxTemp.bits.u32Chksum = v->rxTemp.words.W0 +v->rxTemp.words.W1 + v->rxTemp.words.W2;
            wrSpiPack(&v->rxTemp);
            break;
        case _CMD_READ_MEMORY_DATA:
        case _CMD_SEND_READ_32BITS:
            v->u32OkCnts++;
            break;
        default:
            break;
        }
        v->u32Fsm = _IS_THRER_A_SPI_PACK;
        break;

    case _CLEAR_SPI_TXRX_PACK:
        rstRxFiFoEmpty();
        rstTxFiFoEmpty();
        v->u32Fsm = _IS_THRER_A_SPI_PACK;
        break;
    default:
        if(v->u32Fsm & _MARK_ERROR_SPI_PACK) {
            v->u32Fsm = _CLEAR_SPI_TXRX_PACK;
        }
        break;
    }
}

static inline void receiveSpiPack(HAL_SPI_SLAVE v)
{
    while(isRxEmpty()) {
        if(0<v->s16Rsize){
            v->rxTemp.Dword[v->s16Rsize-1] = rdSpiData();
            v->u32TimeMark = v->u32TimeStamp = U32_UPCNTS;
            v->s16Rsize--;
        }
        else {
            // Out of range of v->s16Rsize
            rdSpiData();
            break;
        }
    }
}


static inline void parseSpiPack(HAL_SPI_SLAVE v)
{
    switch(v->rxTemp.bits.u32Id) {
    case _CMD_WRITE_32BITS:
        v->rxTemp.bits.u32Chksum -= (v->rxTemp.words.W0 +v->rxTemp.words.W1 + v->rxTemp.words.W2);
        if((0 == v->rxTemp.bits.u32Chksum)&&(0 != v->rxTemp.words.W2)) {
            v->u32OkCnts++;
        }
        else {
            v->u32ErrCnts++;
        }
        break;
    case _CMD_GET_READ_HEADER:
        v->rxTemp.bits.u32Data = v->p32Data[v->rxTemp.bits.u32Addr];
        v->rxTemp.bits.u32Chksum = v->rxTemp.words.W0 +v->rxTemp.words.W1 + v->rxTemp.words.W2;
        wrSpiPack(&v->rxTemp);
        v->u32OkCnts++;
        break;
    case _CMD_SEND_READ_32BITS:
        v->u32OkCnts++;
        break;
    default:
        break;
    }

    v->s16Rsize = 4;
}




#endif /* SPI_SLAVE_H_ */
