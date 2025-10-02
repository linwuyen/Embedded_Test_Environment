/*
 * ModbusCommFunc.c
 *
 *  Created on: 2018¦~3¤ë28¤é
 *      Author: user
 */

#include "ModbusMaster.h"
#include "ModbusSci.h"



int16 ReservedFunctionm(SCI_MODBUSm *pMbus)
{
	return MBUS_FAILm;
}

//#pragma CODE_SECTION(readHoldingRegister, "ramfuncs");
int16 readHoldingRegisterm(SCI_MODBUSm *pMbus)
{
	HEADER_READ_HOLDINGREGm *pack = 0;
	U16REGm temp = {0, 0};
	U16REGm *combine = 0;
	Uint16 m_words = 0;
	Uint16 m_addr = 0;

//	while(2 > pMbus->sci->SCIFFTX.bit.TXFFST) {
	while(2 > SCI_getTxFIFOStatus(pMbus->sci)) {

		switch(pMbus->evHoldReg) {

		case _SEND_SLAVEID_AND_FUNCIDm:
			pMbus->tick1msec = 0;
			pMbus->stHoldInfo.tbcrc = DEFAULT_CRC;
			pMbus->stHoldInfo.crc = 0x0000;

//			pMbus->sci->SCITXBUF = pMbus->pData->slave;
			SCI_writeCharNonBlocking(pMbus->sci,pMbus->pData->slave);

			ucMBCRC16m(pMbus->pData->slave, &pMbus->stHoldInfo.tbcrc);

//			pMbus->sci->SCITXBUF = pMbus->pData->function;
			SCI_writeCharNonBlocking(pMbus->sci,pMbus->pData->function);

			ucMBCRC16m(pMbus->pData->function, &pMbus->stHoldInfo.tbcrc);
			pMbus->evHoldReg = _SEND_READING_FROM_ADDRESSm;
			break;

		case _SEND_READING_FROM_ADDRESSm:
			pMbus->tick1msec = 0;
//			m_addr = pMbus->pData->address + 40000;
			m_addr = pMbus->pData->address;
			temp.HiByte = (m_addr>>8)&0x00FF;

//			pMbus->sci->SCITXBUF = temp.HiByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.HiByte);

			ucMBCRC16m(temp.HiByte, &pMbus->stHoldInfo.tbcrc);
			temp.LoByte = m_addr&0x00FF;

//			pMbus->sci->SCITXBUF = temp.LoByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.LoByte);

			ucMBCRC16m(temp.LoByte, &pMbus->stHoldInfo.tbcrc);
			pMbus->evHoldReg = _SEND_HOW_MANY_READING_POINTSm;
			break;

		case _SEND_HOW_MANY_READING_POINTSm:
			pMbus->tick1msec = 0;
			temp.HiByte = (pMbus->pData->points>>8)&0x00FF;

//			pMbus->sci->SCITXBUF = temp.HiByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.HiByte);

			ucMBCRC16m(temp.HiByte, &pMbus->stHoldInfo.tbcrc);
			temp.LoByte = pMbus->pData->points&0x00FF;

//			pMbus->sci->SCITXBUF = temp.LoByte;
		    SCI_writeCharNonBlocking(pMbus->sci,temp.LoByte);


			pMbus->stHoldInfo.crc = ucMBCRC16m(temp.LoByte, &pMbus->stHoldInfo.tbcrc);
			pMbus->evHoldReg = _SEND_CRC_OUTm;
			break;

		case _SEND_CRC_OUTm:
			pMbus->tick1msec = 0;

//			pMbus->sci->SCITXBUF = (pMbus->stHoldInfo.crc>>0) & 0x00FF;
			SCI_writeCharNonBlocking(pMbus->sci, (pMbus->stHoldInfo.crc & 0x00FF));

//			pMbus->sci->SCITXBUF = (pMbus->stHoldInfo.crc>>8)&0x00FF;
			SCI_writeCharNonBlocking(pMbus->sci, (pMbus->stHoldInfo.crc >> 8 & 0x00FF));

			pMbus->tick1msec = 0;
			pMbus->stHoldInfo.tbcrc = DEFAULT_CRC;
			pMbus->stHoldInfo.crc = 0x0000;
			pMbus->stHoldInfo.pRX = &pMbus->RxBuffer[0];
			pMbus->evHoldReg = _RECEIVE_ALL_OF_DATAm;
			break;

		case _RECEIVE_ALL_OF_DATAm:
//			if(0 < pMbus->sci->SCIFFRX.bit.RXFFST){
			if(0 < SCI_getRxFIFOStatus(pMbus->sci)){

				pMbus->tick1msec = 0;

//				*pMbus->stHoldInfo.pRX = pMbus->sci->SCIRXBUF.all & 0x00FF;
				*pMbus->stHoldInfo.pRX = SCI_readCharNonBlocking(pMbus->sci);

				pMbus->stHoldInfo.crc = ucMBCRC16m(*pMbus->stHoldInfo.pRX, &pMbus->stHoldInfo.tbcrc);
				pMbus->stHoldInfo.pRX++;
				if(0 == pMbus->stHoldInfo.crc) 	pMbus->evHoldReg = _PROCESS_ALL_OF_DATAm;
			}

			return MBUS_WAITm;

		case _PROCESS_ALL_OF_DATAm:
			pMbus->tick1msec = 0;
			pack = (HEADER_READ_HOLDINGREGm *)&pMbus->RxBuffer[0];
			pMbus->evHoldReg = _SEND_SLAVEID_AND_FUNCIDm;
			if(0x0080&pack->Function) return MBUS_FAILm;
			else {
				combine = &pack->Data;
				m_words = pack->ByteCount/2;
				for(pMbus->stHoldInfo.m_index = 0; pMbus->stHoldInfo.m_index < m_words; pMbus->stHoldInfo.m_index++, combine++) {
					pMbus->pData->reg[pMbus->stHoldInfo.m_index] = ((combine->HiByte<<8)&0xFF00) | (combine->LoByte&0x00FF);
				}
				return MBUS_SUCCESSm;
			}

		default:
			break;
		}
	}

	return MBUS_WAITm;
}

//#pragma CODE_SECTION(writeHoldingRegister, "ramfuncs");
int16 writeHoldingRegisterm(SCI_MODBUSm *pMbus)
{
	HEADER_READ_HOLDINGREGm *pack = 0;
	U16REGm temp = {0, 0};
	Uint16 m_addr = 0;

//	while(2 > pMbus->sci->SCIFFTX.bit.TXFFST) {
	while(2 > SCI_getTxFIFOStatus(pMbus->sci)) {

		switch(pMbus->evHoldReg) {

		case _SEND_SLAVEID_AND_FUNCIDm:
			pMbus->tick1msec = 0;
			pMbus->stHoldInfo.tbcrc = DEFAULT_CRC;
			pMbus->stHoldInfo.crc = 0x0000;

//			pMbus->sci->SCITXBUF = pMbus->pData->slave;
			SCI_writeCharNonBlocking(pMbus->sci,pMbus->pData->slave);

			ucMBCRC16m(pMbus->pData->slave, &pMbus->stHoldInfo.tbcrc);

//			pMbus->sci->SCITXBUF = pMbus->pData->function;
			SCI_writeCharNonBlocking(pMbus->sci,pMbus->pData->function);

			ucMBCRC16m(pMbus->pData->function, &pMbus->stHoldInfo.tbcrc);

			pMbus->evHoldReg = _SEND_READING_FROM_ADDRESSm;
			break;

		case _SEND_READING_FROM_ADDRESSm:
			pMbus->tick1msec = 0;
//			m_addr = pMbus->pData->address + 40000;
			m_addr = pMbus->pData->address;

			temp.HiByte = (m_addr>>8)&0x00FF;

//			pMbus->sci->SCITXBUF = temp.HiByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.HiByte);

			ucMBCRC16m(temp.HiByte, &pMbus->stHoldInfo.tbcrc);
			temp.LoByte = m_addr&0x00FF;

//			pMbus->sci->SCITXBUF = temp.LoByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.LoByte);

			ucMBCRC16m(temp.LoByte, &pMbus->stHoldInfo.tbcrc);
			pMbus->evHoldReg = _SEND_A_DATA_OUTm;
			break;

		case _SEND_A_DATA_OUTm:
			pMbus->tick1msec = 0;

			temp.HiByte = (pMbus->pData->reg[0]>>8)&0x00FF;

//			pMbus->sci->SCITXBUF = temp.HiByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.HiByte);

			ucMBCRC16m(temp.HiByte, &pMbus->stHoldInfo.tbcrc);

			temp.LoByte = pMbus->pData->reg[0]&0x00FF;

//			pMbus->sci->SCITXBUF = temp.LoByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.LoByte);

			pMbus->stHoldInfo.crc = ucMBCRC16m(temp.LoByte, &pMbus->stHoldInfo.tbcrc);
			pMbus->evHoldReg = _SEND_CRC_OUTm;
			break;

		case _SEND_CRC_OUTm:
			pMbus->tick1msec = 0;

//			pMbus->sci->SCITXBUF = (pMbus->stHoldInfo.crc>>0)&0x00FF;
			SCI_writeCharNonBlocking(pMbus->sci, (pMbus->stHoldInfo.crc & 0x00FF));

//			pMbus->sci->SCITXBUF = (pMbus->stHoldInfo.crc>>8)&0x00FF;
			SCI_writeCharNonBlocking(pMbus->sci, (pMbus->stHoldInfo.crc >> 8 & 0x00FF));


			pMbus->stHoldInfo.tbcrc = DEFAULT_CRC;
			pMbus->stHoldInfo.crc = 0x0000;
			pMbus->stHoldInfo.pRX = &pMbus->RxBuffer[0];

			if( pMbus->pData->slave == 0 )
			{
                pMbus->evHoldReg = _SEND_SLAVEID_AND_FUNCIDm;
                return MBUS_SUCCESSm;
            }
            else
            {
                pMbus->evHoldReg = _RECEIVE_ALL_OF_DATAm;
            }
			break;

		case _RECEIVE_ALL_OF_DATAm:
//			if(0 < pMbus->sci->SCIFFRX.bit.RXFFST){



		    if(0 < SCI_getRxFIFOStatus(pMbus->sci)){

				pMbus->tick1msec = 0;

//				*pMbus->stHoldInfo.pRX = pMbus->sci->SCIRXBUF.all & 0x00FF;
				*pMbus->stHoldInfo.pRX = SCI_readCharNonBlocking(pMbus->sci);

				pMbus->stHoldInfo.crc = ucMBCRC16m(*pMbus->stHoldInfo.pRX, &pMbus->stHoldInfo.tbcrc);
				pMbus->stHoldInfo.pRX++;
				if(0 == pMbus->stHoldInfo.crc) 	pMbus->evHoldReg = _PROCESS_ALL_OF_DATAm;
			}
			return MBUS_WAITm;

		case _PROCESS_ALL_OF_DATAm:
			pMbus->tick1msec = 0;
			pack = (HEADER_READ_HOLDINGREGm *)&pMbus->RxBuffer[0];
			pMbus->evHoldReg = _SEND_SLAVEID_AND_FUNCIDm;
			if(0x0080&pack->Function) return MBUS_FAILm;
			else return MBUS_SUCCESSm;

		default:
			break;
		}
	}

	return MBUS_WAITm;
}

//#pragma CODE_SECTION(writeHoldingNRegister, "ramfuncs");
int16 writeHoldingNRegisterm(SCI_MODBUSm *pMbus)
{
	HEADER_READ_HOLDINGREGm *pack = 0;
	U16REGm temp = {0, 0};
	Uint16 m_addr = 0;

//	while(2 > pMbus->sci->SCIFFTX.bit.TXFFST) {
	while(2 > SCI_getTxFIFOStatus(pMbus->sci)) {

		switch(pMbus->evHoldReg) {

		case _SEND_SLAVEID_AND_FUNCIDm:
			pMbus->tick1msec = 0;
			pMbus->stHoldInfo.tbcrc = DEFAULT_CRC;
			pMbus->stHoldInfo.crc = 0x0000;
			pMbus->stHoldInfo.m_index = 0;

//			pMbus->sci->SCITXBUF = pMbus->pData->slave;
			SCI_writeCharNonBlocking(pMbus->sci,pMbus->pData->slave);

			ucMBCRC16m(pMbus->pData->slave, &pMbus->stHoldInfo.tbcrc);

//			pMbus->sci->SCITXBUF = pMbus->pData->function;
			SCI_writeCharNonBlocking(pMbus->sci,pMbus->pData->function);

			ucMBCRC16m(pMbus->pData->function, &pMbus->stHoldInfo.tbcrc);
			pMbus->evHoldReg = _SEND_READING_FROM_ADDRESSm;
			break;

		case _SEND_READING_FROM_ADDRESSm:
			pMbus->tick1msec = 0;
//			m_addr = pMbus->pData->address + 40000;
			m_addr = pMbus->pData->address;

			temp.HiByte = (m_addr>>8)&0x00FF;

//			pMbus->sci->SCITXBUF = temp.HiByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.HiByte);

			ucMBCRC16m(temp.HiByte, &pMbus->stHoldInfo.tbcrc);
			temp.LoByte = m_addr&0x00FF;

//			pMbus->sci->SCITXBUF = temp.LoByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.LoByte);

			ucMBCRC16m(temp.LoByte, &pMbus->stHoldInfo.tbcrc);
			pMbus->evHoldReg = _SEND_HOW_MANY_READING_POINTSm;
			break;

		case _SEND_HOW_MANY_READING_POINTSm:
			pMbus->tick1msec = 0;
			temp.HiByte = (pMbus->pData->points>>8)&0x00FF;

//			pMbus->sci->SCITXBUF = temp.HiByte;
		    SCI_writeCharNonBlocking(pMbus->sci,temp.HiByte);

			ucMBCRC16m(temp.HiByte, &pMbus->stHoldInfo.tbcrc);
			temp.LoByte = pMbus->pData->points&0x00FF;

//			pMbus->sci->SCITXBUF = temp.LoByte;
			SCI_writeCharNonBlocking(pMbus->sci,temp.LoByte);

			ucMBCRC16m(temp.LoByte, &pMbus->stHoldInfo.tbcrc);
			pMbus->evHoldReg = _SEND_BYTE_COUNTSm;
			break;

		case _SEND_BYTE_COUNTSm:
			pMbus->tick1msec = 0;
//			pMbus->sci->SCITXBUF = pMbus->pData->bytes;

			SCI_writeCharNonBlocking(pMbus->sci,pMbus->pData->bytes);
			ucMBCRC16m(pMbus->pData->bytes, &pMbus->stHoldInfo.tbcrc);

//	        SCI_writeCharNonBlocking(pMbus->sci,pMbus->pData->bytes);
//	        ucMBCRC16m(pMbus->pData->bytes, &pMbus->stHoldInfo.tbcrc);

			pMbus->evHoldReg = _SEND_ALL_OF_DATAm;
			break;

		case _SEND_ALL_OF_DATAm:
			if(pMbus->stHoldInfo.m_index < pMbus->pData->points) {
				pMbus->tick1msec = 0;
				temp.HiByte = (pMbus->pData->reg[pMbus->stHoldInfo.m_index]>>8)&0x00FF;

//				pMbus->sci->SCITXBUF = temp.HiByte;
				SCI_writeCharNonBlocking(pMbus->sci,temp.HiByte);

				ucMBCRC16m(temp.HiByte, &pMbus->stHoldInfo.tbcrc);
				temp.LoByte = pMbus->pData->reg[pMbus->stHoldInfo.m_index]&0x00FF;

//				pMbus->sci->SCITXBUF = temp.LoByte;
				SCI_writeCharNonBlocking(pMbus->sci,temp.LoByte);

				pMbus->stHoldInfo.crc = ucMBCRC16m(temp.LoByte, &pMbus->stHoldInfo.tbcrc);
				pMbus->stHoldInfo.m_index++;
			}
			else pMbus->evHoldReg = _SEND_CRC_OUTm;
			break;

		case _SEND_CRC_OUTm:
			pMbus->tick1msec = 0;

//			pMbus->sci->SCITXBUF = (pMbus->stHoldInfo.crc>>0)&0x00FF;
			SCI_writeCharNonBlocking(pMbus->sci, (pMbus->stHoldInfo.crc & 0x00FF));

//			pMbus->sci->SCITXBUF = (pMbus->stHoldInfo.crc>>8)&0x00FF;
			SCI_writeCharNonBlocking(pMbus->sci, (pMbus->stHoldInfo.crc >> 8 & 0x00FF));

			pMbus->stHoldInfo.tbcrc = DEFAULT_CRC;
			pMbus->stHoldInfo.crc = 0x0000;
			pMbus->stHoldInfo.pRX = &pMbus->RxBuffer[0];
			pMbus->evHoldReg = _RECEIVE_ALL_OF_DATAm;
			break;

		case _RECEIVE_ALL_OF_DATAm:
//			if(0 < pMbus->sci->SCIFFRX.bit.RXFFST){
		    if(0 < SCI_getRxFIFOStatus(pMbus->sci)){

				pMbus->tick1msec = 0;

//				*pMbus->stHoldInfo.pRX = pMbus->sci->SCIRXBUF.all & 0x00FF;
				*pMbus->stHoldInfo.pRX = SCI_readCharNonBlocking(pMbus->sci);

				pMbus->stHoldInfo.crc = ucMBCRC16m(*pMbus->stHoldInfo.pRX, &pMbus->stHoldInfo.tbcrc);
				pMbus->stHoldInfo.pRX++;
				if(0 == pMbus->stHoldInfo.crc) pMbus->evHoldReg = _PROCESS_ALL_OF_DATAm;
			}
			return MBUS_WAITm;

		case _PROCESS_ALL_OF_DATAm:
			pMbus->tick1msec = 0;
			pack = (HEADER_READ_HOLDINGREGm *)&pMbus->RxBuffer[0];
			pMbus->evHoldReg = _SEND_SLAVEID_AND_FUNCIDm;
			if(0x0080&pack->Function) return MBUS_FAILm;
			else return MBUS_SUCCESSm;

		default:
			break;
		}
	}
	return MBUS_WAITm;
}


int16 (*ModbusFuncm[25])(SCI_MODBUSm *pMbus) = {
		ReservedFunctionm,		//0x00
		ReservedFunctionm,		//0x01
		ReservedFunctionm,		//0x02
		readHoldingRegisterm,		//0x03		Read Holding Registers
		ReservedFunctionm,		//0x04
		ReservedFunctionm,		//0x05
		writeHoldingRegisterm,		//0x06		Preset Single Register
		ReservedFunctionm,		//0x07
		ReservedFunctionm,		//0x08
		ReservedFunctionm,		//0x09
		ReservedFunctionm,		//0x0A
		ReservedFunctionm,		//0x0B
		ReservedFunctionm,		//0x0C
		ReservedFunctionm,		//0x0D
		ReservedFunctionm,		//0x0E
		ReservedFunctionm,		//0x0F
		writeHoldingNRegisterm,		//0x10		 Preset Multiple Regs
		ReservedFunctionm,		//0x11
		ReservedFunctionm,		//0x12
		ReservedFunctionm,		//0x13
		ReservedFunctionm,		//0x14
		ReservedFunctionm,		//0x15
		ReservedFunctionm,		//0x16
		ReservedFunctionm,		//0x17
		ReservedFunctionm		//0x18

};

Uint16 DisplayRegister[8] = {'8', '8','8','8','8'};
Uint16 KeyRegister[2] = {0, 0};

SCI_MODBUSm mbcomm_m = DEFAULT_DISPMODBUS;


