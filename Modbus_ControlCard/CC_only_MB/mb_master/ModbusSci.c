/*
 * ModbusSci.c
 *
 *  Created on: 2018¦~3¤ë28¤é
 *      Author: user
 */

#include "ModbusMaster.h"
#include "ModbusSci.h"


void cleanSciX(volatile struct SCI_REGS *psci)
{
	psci->SCIFFRX.bit.RXFIFORESET = 0;
	psci->SCIFFTX.bit.TXFIFOXRESET = 0;
}

void resetSciX(volatile struct SCI_REGS *psci)
{
	psci->SCIFFRX.bit.RXFIFORESET = 1;
	psci->SCIFFTX.bit.TXFIFOXRESET = 1;
}

int16 initSciX(Uint32 baudrate, Uint16 databits, Uint16 parity, volatile struct SCI_REGS *psci)
{
	Uint32 UxBR = (Uint32) ( LOW_SPEED_CLOCK / ((baudrate+1)*8) );
	// Number of bytes

	switch(databits) {
		case 8:
			psci->SCICCR.bit.SCICHAR = 0x7;
			break;
		case 7:
			psci->SCICCR.bit.SCICHAR = 0x6;
			break;
		default:
			psci->SCICCR.bit.SCICHAR = 0x7;
	}

	// Parity settings
	switch(parity){
		case SERIAL_PARITY_EVEN:
			psci->SCICCR.bit.PARITYENA = 1;
			psci->SCICCR.bit.PARITY = 1;
			break;
		case SERIAL_PARITY_ODD:
			psci->SCICCR.bit.PARITYENA = 1;
			psci->SCICCR.bit.PARITY = 0;
			break;
		case SERIAL_PARITY_NONE:
			psci->SCICCR.bit.PARITYENA = 0;
			break;
		default:
			psci->SCICCR.bit.PARITYENA = 0;
	}


	// Configure the High and Low baud rate registers
	psci->SCIHBAUD    = ((UxBR>>8)&0x00FF);
	psci->SCILBAUD    = ((UxBR>>0)&0x00FF);

	psci->SCICTL1.all =0x0023;     // Relinquish SCI from Reset

	psci->SCIFFTX.all=0xE040;
	psci->SCIFFRX.all=0x2044;
	psci->SCIFFCT.all=0x0;

	psci->SCICTL1.bit.SWRESET = 0;

	psci->SCICTL1.bit.SWRESET = 1;
	psci->SCIFFRX.bit.RXFIFORESET = 1;
	psci->SCIFFTX.bit.TXFIFOXRESET = 1;

	return 0;
}



