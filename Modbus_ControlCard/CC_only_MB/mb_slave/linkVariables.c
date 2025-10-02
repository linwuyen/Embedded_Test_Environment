/*
 *  File Name: linkVariables.c
 *
 *  Created on: 8/26/2025
 *  Author: POWER-532A86
 */

#include "ModbusCommon.h"
#include "ModbusSlave.h"


void initRegN(void *v){ 

	SCI_MODBUS *p = (SCI_MODBUS *) v;
	p->pReg->u16COMmonHEAder = 2423; 
	p->pReg->u16COMmonLENgth = 4; 
	p->pReg->u16MAChineINFOrmationOFFSet = 9000; 
	p->pReg->u16USERPARameterOFFSet = 1000; 
	p->pReg->u16ADVAncePARameterOFFSet = 1020; 
	p->pReg->u16COMmonCHEcksum = 13447; 
	p->pReg->u16DACCHAnnel = 1; 
	p->pReg->u16ENABleel = 0; 
	p->pReg->u16ELOAdVONENABle = 1; 
	p->pReg->u32FIRmwareBUIldDATe = 2023083110; 
	p->pReg->f32VOLtageLOOpkp = 2; 
	p->pReg->f32VOLtageLOOpki = 0.02; 
	p->pReg->f32VOLtageLOOpMAXimum = 1; 
	p->pReg->f32VOLtageLOOpMINimum = -1; 
	p->pReg->f32CURrentLOOpkp = 1; 
	p->pReg->f32CURrentLOOpki = 0.1; 
	p->pReg->f32CURrentLOOpMAXimum = 1; 
	p->pReg->f32CURrentLOOpMINimum = -1; 
	p->pReg->u16COUntryCODe = 886; 
	p->pReg->u16VENderid = 2423; 
	p->pReg->u32BUIldDATe = 2023083110; 
}

void readRegN(void *v){ 

	SCI_MODBUS *p = (SCI_MODBUS *) v;
}

void writeReg(void *v){ 

	SCI_MODBUS *p = (SCI_MODBUS *) v;
	switch(p->info.rwfrom) {
	default:
	    break;
	}
}

void writeRegN(void *v){ 

	SCI_MODBUS *p = (SCI_MODBUS *) v;
}

