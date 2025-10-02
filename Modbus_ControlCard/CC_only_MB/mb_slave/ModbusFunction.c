/*
 * ModbusCommFunc.c
 *
 *  Created on: 2018¦~3¤ë28¤é
 *      Author: user
 */

#include "ModbusCommon.h"
#include "ModbusSlave.h"

#define HBYTE(x) ((x>>8)&0x00FF)
#define LBYTE(x) (x&0x00FF)

int16_t pushModbusByte(uint16_t u16Data, SCI_MODBUS *mbus)
{
    if(BROADCAST_ID != mbus->sFiFo.u16RAM[0]) {
        mbus->sFiFo.u16RAM[mbus->sFiFo.pushTcnts++] = u16Data;
        return ucMBCRC16(u16Data, &mbus->info.tbcrc);
    }
    else {
        //Broadcast
        return 0;
    }
}

int16_t ReservedFunction(SCI_MODBUS *mbus)
{
	mbus->errmsg = MB_ERROR_ILLEGALFUNC;
	return MBUS_FAIL;
}

int16_t readHoldingRegister(SCI_MODBUS *mbus)
{
	static HEADER_READ_HOLDINGREG *pack = 0;
	static uint16_t u16Word = 0;

    pack = (HEADER_READ_HOLDINGREG *)mbus->pHeader;

    switch(mbus->evFunc) {
    case _SEND_ADDRESS:
        mbus->info.rwfrom = (pack->Address.HiByte<<8) | pack->Address.LoByte;

        mbus->errmsg = (MSG_ERROR_ID)chkValidAddress(mbus->info.rwfrom );
        if(MB_NO_ERROR ==  mbus->errmsg) {
            pushModbusByte(pack->Slave, mbus);
            pushModbusByte(pack->Function, mbus);
            mbus->evFunc = _SEND_BYTE_COUNTS;
            //The number of bytes convert to the number of words.
            mbus->info.words = (((pack->Points.HiByte<<8)&0xFF00) | pack->Points.LoByte);
        }
        else {
            return MBUS_FAIL;
        }

    case _SEND_BYTE_COUNTS:
        pushModbusByte(mbus->info.words*2, mbus);
        mbus->getReg((void*)mbus);
        mbus->evFunc = _SEND_DATA_OUT;
        break;

    case _SEND_DATA_OUT:
        if(mbus->info.index<mbus->info.words) {
            u16Word = getModbusData(mbus->info.rwfrom);
            pushModbusByte(HBYTE(u16Word), mbus);
            mbus->info.crc = pushModbusByte(LBYTE(u16Word), mbus);
            mbus->info.index++;
            mbus->info.rwfrom++;
        }
        else {
            pushModbusByte(LBYTE(mbus->info.crc), mbus);
            pushModbusByte(HBYTE(mbus->info.crc), mbus);
            mbus->evFunc = _SEND_ADDRESS;
            return MBUS_SUCCESS;
        }
        break;

    default:
        mbus->info = DEFAULT_REG_INFO;
        mbus->evFunc = _SEND_ADDRESS;
        mbus->errmsg = MB_ERROR_SLVFAILURE;
        break;
    }

    if(MB_NO_ERROR != mbus->errmsg) return MBUS_FAIL;


	return MBUS_WAIT;
}


int16_t writeHoldingRegister(SCI_MODBUS *mbus)
{
    static HEADER_WRITE_HOLDINGREG *pack = 0;
    static uint16_t u16Word = 0;

    pack = (HEADER_WRITE_HOLDINGREG *)mbus->pHeader;

    switch(mbus->evFunc) {
    case _SEND_ADDRESS:
        mbus->info.rwfrom = (pack->Address.HiByte<<8) | pack->Address.LoByte;

        mbus->errmsg = (MSG_ERROR_ID)chkValidAddress(mbus->info.rwfrom );
        if(MB_NO_ERROR ==  mbus->errmsg) {
            pushModbusByte(pack->Slave, mbus);
            pushModbusByte(pack->Function, mbus);
            mbus->evFunc = _SEND_MEMADDR;
            mbus->info.words = 1;
            // Get the start of Data.
            mbus->info.pData = &pack->Data;
        }
        else {
            return MBUS_FAIL;
        }

    case _SEND_MEMADDR:
        pushModbusByte(pack->Address.HiByte, mbus);
        pushModbusByte(pack->Address.LoByte, mbus);
        mbus->evFunc = _SEND_DATA_OUT;

    case _SEND_DATA_OUT:
        if(mbus->info.index<mbus->info.words) {
            u16Word = (uint16_t)((mbus->info.pData[mbus->info.index].HiByte<<8)&0xFF00)|(mbus->info.pData[mbus->info.index].LoByte&0x00FF);
            setModbusData(mbus->info.rwfrom, u16Word);
            pushModbusByte(mbus->info.pData[mbus->info.index].HiByte, mbus);
            mbus->info.crc = pushModbusByte(mbus->info.pData[mbus->info.index].LoByte, mbus);
            mbus->info.index++;
        }
        else {
            pushModbusByte(LBYTE(mbus->info.crc), mbus);
            pushModbusByte(HBYTE(mbus->info.crc), mbus);
            mbus->evFunc = _SEND_ADDRESS;
            mbus->setReg((void*)mbus);
            return MBUS_SUCCESS;
        }
        break;

    default:
        mbus->info = DEFAULT_REG_INFO;
        mbus->evFunc = _SEND_ADDRESS;
        mbus->errmsg = MB_ERROR_SLVFAILURE;
        break;
    }

    if(MB_NO_ERROR != mbus->errmsg) return MBUS_FAIL;

    return MBUS_WAIT;
}


int16_t writeHoldingNRegister(SCI_MODBUS *mbus)
{
	static HEADER_WRITE_N_HOLDINGREG *pack = 0;
	static uint16_t u16Word = 0;

    pack = (HEADER_WRITE_N_HOLDINGREG *)mbus->pHeader;

    switch(mbus->evFunc) {
    case _SEND_ADDRESS:

        mbus->info.rwfrom = (pack->Address.HiByte<<8) | pack->Address.LoByte;

        mbus->errmsg = (MSG_ERROR_ID)chkValidAddress(mbus->info.rwfrom );
        if(MB_NO_ERROR ==  mbus->errmsg) {
            pushModbusByte(pack->Slave, mbus);
            pushModbusByte(pack->Function, mbus);
            mbus->evFunc = _SEND_MEMADDR;
            //The number of bytes convert to the number of words.
            mbus->info.words = (((pack->Points.HiByte<<8)&0xFF00) | pack->Points.LoByte);
            // Get the start of Data.
            mbus->info.pData = &pack->Data;
        }
        else {
            return MBUS_FAIL;
        }

    case _SEND_MEMADDR:
        pushModbusByte(pack->Address.HiByte, mbus);
        pushModbusByte(pack->Address.LoByte, mbus);
        mbus->evFunc = _SEND_MEMSIZE;

    case _SEND_MEMSIZE:
        pushModbusByte(pack->Points.HiByte, mbus);
        mbus->info.crc = pushModbusByte(pack->Points.LoByte, mbus);
        mbus->evFunc = _SAVE_BYTE_COUNTS;

    case _SAVE_BYTE_COUNTS:
        if(mbus->info.index<mbus->info.words) {
            u16Word = (uint16_t)((mbus->info.pData[mbus->info.index].HiByte<<8)&0xFF00)|(mbus->info.pData[mbus->info.index].LoByte&0x00FF);
            setModbusData(mbus->info.rwfrom, u16Word);
            mbus->info.index++;
            mbus->info.rwfrom++;
        }
        else {
            pushModbusByte(LBYTE(mbus->info.crc), mbus);
            pushModbusByte(HBYTE(mbus->info.crc), mbus);
            mbus->evFunc = _SEND_ADDRESS;
            mbus->setRegN((void*)mbus);
            return MBUS_SUCCESS;
        }
        break;

    default:
        mbus->info = DEFAULT_REG_INFO;
        mbus->evFunc = _SEND_ADDRESS;
        mbus->errmsg = MB_ERROR_SLVFAILURE;
        break;
    }

    if(MB_NO_ERROR != mbus->errmsg) return MBUS_FAIL;

	return MBUS_WAIT;
}


int16_t (*ModbusFunc[MB_END_OF_FUNCID])(SCI_MODBUS *mbus) = {
		ReservedFunction,		//0x00
		ReservedFunction,		//0x01
		ReservedFunction,		//0x02
		readHoldingRegister,	//0x03		Read Holding Registers
		ReservedFunction,		//0x04
		ReservedFunction,		//0x05
		writeHoldingRegister,	//0x06		Write Single Register
		ReservedFunction,		//0x07
		ReservedFunction,		//0x08
		ReservedFunction,		//0x09
		ReservedFunction,		//0x0A
		ReservedFunction,		//0x0B
		ReservedFunction,		//0x0C
		ReservedFunction,		//0x0D
		ReservedFunction,		//0x0E
		ReservedFunction,		//0x0F
		writeHoldingNRegister,	//0x10		Write Multiple Register
		ReservedFunction,		//0x11
		ReservedFunction,		//0x12
		ReservedFunction,		//0x13
		ReservedFunction,		//0x14
		ReservedFunction,		//0x15
		ReservedFunction,		//0x16
		ReservedFunction,		//0x17
		ReservedFunction		//0x18

};


int16_t ModbusCommError(SCI_MODBUS *mbus)
{
    static MODBUS_HEADER *pack = 0;
    static uint16_t u16ErrFunc = 0;

    pack = (MODBUS_HEADER *)mbus->pHeader;

    if(MB_NO_ERROR == mbus->errmsg) return MB_NO_ERROR;

    mbus->info.tbcrc = DEFAULT_CRCs;
    mbus->info.crc = 0;
    pushModbusByte(pack->Slave, mbus);


    u16ErrFunc = pack->Function | MB_ERROR_MASK;
    pushModbusByte(u16ErrFunc, mbus);

    if(SIZE_OF_ERRORLOG > mbus->amount_of_error) {
        mbus->errorlog[mbus->amount_of_error] = mbus->errmsg;
        mbus->amount_of_error++;
    }
    mbus->info.crc = pushModbusByte(mbus->errmsg, mbus);

    pushModbusByte(mbus->info.crc & 0x00FF, mbus);
    pushModbusByte((mbus->info.crc>>8) & 0x00FF, mbus);
    mbus->errmsg = MB_NO_ERROR;
    mbus->evError = _SEND_ADDRESS;

	return MBUS_SEND_ERROR_MSG_BACK;
}

void initRegN(void *p);
void readRegN(void *p);
void writeReg(void *p);
void writeRegN(void *p);


volatile SCI_MODBUS mbcomm_s = {
		.pHeader = (MODBUS_HEADER *)&mbcomm_s.sFiFo.u16RAM[0],
		//Configure Serial Port
		.sci = DEBUG_SCI_BASE,
		.getRsize = (int16_t (*)(uint32_t) )SCI_getRxFIFOStatus,
	    .rdfunc = SCI_readCharNonBlocking,
	    .getWsize = (int16_t (*)(uint32_t)) SCI_getTxFIFOStatus,
	    .wrfunc = SCI_writeCharNonBlocking,
	    //Configure Modbus
//		.slaveid = 0x03,
		.slaveid = 0x00,
//	    .slaveid = Conn.GBslaveid
		.pReg = &regMbusData,
		.initReg = initRegN,
		.getReg = readRegN,
		.setReg = writeReg,
		.setRegN = writeRegN,

};


