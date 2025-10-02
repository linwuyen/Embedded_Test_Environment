/*
 * ModbusSlave.h
 *
 *  Created on: 2018¦~3¤ë26¤é
 *      Author: user
 */

#ifndef MODBUSMASTER_H_
#define MODBUSMASTER_H_


#include "ModbusCommonm.h"
#include "ModbusSci.h"     // Device Headerfile and Examples Include File



typedef enum {
    MBUS_SUCCESSm = 1,
    MBUS_WAITm   =  0,
    MBUS_FAILm   = -1,
    MBUS_RESETm   = 2,
    PARSER_TIMEOUTenum = 3
} MODBUS_STATUSm;

typedef struct {
	Uint16 HiByte;
	Uint16 LoByte;
} U16REGm;

typedef struct {
	Uint16 Slave;
	Uint16 Function;
	U16REGm Address;
} MODBUS_HEADERm;

typedef struct {
	Uint16 Slave;
	Uint16 Function;
	Uint16 ByteCount;
	U16REGm Data;
} HEADER_READ_HOLDINGREGm;


typedef struct {
	Uint16 Slave;
	Uint16 Function;
	U16REGm Address;
	U16REGm Data;
} HEADER_WRITE_HOLDINGREGm;

typedef struct {
	Uint16 Slave;
	Uint16 Function;
	U16REGm Address;
	U16REGm Points;
	Uint16 Bytes;
	U16REGm Data;
} HEADER_WRITE_N_HOLDINGREGm;

typedef struct {
	Uint16 Slave;
	Uint16 Function;
	Uint16 ErrorNo;
	U16REGm CRC;
} HEADER_ERROR_REGm;


typedef enum{
	_INIT_SCI_GPIOm = 0,
	_INIT_SCI_CONFIGm,
	_POP_COMMAND_OUTm,
	_EXE_RW_COMMANDm,
	_SAVE_ERROR_LOGm,
	_PROCESS_TIMEOUTm,
	_RESET_FIFO_DATAm
} EV_MODBUSm;

#define DEFAULT_SCI_MODBUS (SCI_MODBUSm) { \
		.evstep = _INIT_SCI_GPIOm, \
		.state = 0}

typedef struct {
	Uint16 slave;
	Uint16 function;
	Uint16 address;
	Uint16 points;
	Uint16 bytes;
	Uint16 *reg;
} CMDPACK;

#define DEFAULT_CMDPACK (CMDPACK) { \
			.slave = 0, \
			.function = 0, \
			.address = 0, \
			.points = 0, \
			.bytes = 0, \
			.reg = 0 }

typedef enum {
	_SEND_SLAVEID_AND_FUNCIDm = 0,
	_SEND_READING_FROM_ADDRESSm,
	_SEND_HOW_MANY_READING_POINTSm,
	_SEND_BYTE_COUNTSm,
	_SEND_ALL_OF_DATAm,
	_SEND_A_DATA_OUTm,
	_SEND_CRC_OUTm,
	_RECEIVE_ALL_OF_DATAm,
	_PROCESS_ALL_OF_DATAm
} EV_HOLDINGREG;

typedef struct {
	ST_CRC tbcrc;
	Uint16 crc;
	Uint16 *pRX;
	Uint16 m_index;
} ST_HOLDINGINFO;

#define DEFAULT_HOLDINGINFO (ST_HOLDINGINFO) { \
		.tbcrc = DEFAULT_CRC, \
		.crc = 0x0000, \
		.pRX = 0, \
		.m_index = 0 }

typedef struct {
	CMDPACK *pData;
	EV_MODBUSm evstep;	//Event Step;
	MODBUS_STATUSm state;
	EV_HOLDINGREG evHoldReg;
	ST_HOLDINGINFO stHoldInfo;
//	volatile struct SCI_REGS *sci;
	uint32_t sci;
	Uint16 RxBuffer[256];
	Uint32 tick1msec;

} SCI_MODBUSm;

#define DEFAULT_DISPMODBUS (SCI_MODBUSm){ \
	.sci = DEBUG_SCI_BASE, \
	.stHoldInfo = DEFAULT_HOLDINGINFO, \
	.evHoldReg = _SEND_SLAVEID_AND_FUNCIDm }

extern Uint16 DisplayRegister[8];
extern Uint16 KeyRegister[2];
extern SCI_MODBUSm mbcomm_m;

extern int16 pushCmdPack(CMDPACK * src);
extern CMDPACK * popCmdPack(void);
extern int16 exeModbusMaster(SCI_MODBUSm *mbus);

#endif /* MODBUSMASTER_H_ */
