/*
 * ModbusSci.h
 *
 *  Created on: 2018¦~3¤ë28¤é
 *      Author: user
 */

#ifndef MODBUSSCI_H_
#define MODBUSSCI_H_


//#include "DSP2803x_Sci.h"                // SCI Registers

#define LOW_SPEED_CLOCK 	15000000

#define SERIAL_BAUDRATE 	38400
#define SERIAL_PARITY 		SERIAL_PARITY_EVEN
#define SERIAL_BITS_NUMBER 	8

#define SERIAL_START_STOP_NUMBER_BITS	2
#if SERIAL_PARITY == SERIAL_PARITY_NONE
#define SERIAL_PARITY_NUMBER_BITS		0
#else
#define SERIAL_PARITY_NUMBER_BITS		1
#endif

typedef enum {
	SERIAL_PARITY_NONE,
	SERIAL_PARITY_EVEN,
	SERIAL_PARITY_ODD
} SerialParity;

extern void cleanSciX(volatile struct SCI_REGS *psci);
extern void resetSciX(volatile struct SCI_REGS *psci);
extern int16 initSciX(Uint32 baudrate, Uint16 databits, Uint16 parity, volatile struct SCI_REGS *psci);


#define DEBUG_SCI_BASE SCIB_BASE
#define DEBUG_SCI_BAUDRATE 115200
#define DEBUG_SCI_CONFIG_WLEN SCI_CONFIG_WLEN_8
#define DEBUG_SCI_CONFIG_STOP SCI_CONFIG_STOP_ONE
#define DEBUG_SCI_CONFIG_PAR SCI_CONFIG_PAR_EVEN
#define DEBUG_SCI_FIFO_TX_LVL SCI_FIFO_TX0
#define DEBUG_SCI_FIFO_RX_LVL SCI_FIFO_RX0

inline void DEBUG_SCI_init(){
    SCI_clearInterruptStatus(DEBUG_SCI_BASE, SCI_INT_RXFF | SCI_INT_TXFF | SCI_INT_FE | SCI_INT_OE | SCI_INT_PE | SCI_INT_RXERR | SCI_INT_RXRDY_BRKDT | SCI_INT_TXRDY);
    SCI_clearOverflowStatus(DEBUG_SCI_BASE);
    SCI_resetTxFIFO(DEBUG_SCI_BASE);
    SCI_resetRxFIFO(DEBUG_SCI_BASE);
    SCI_resetChannels(DEBUG_SCI_BASE);
    SCI_setConfig(DEBUG_SCI_BASE, DEVICE_LSPCLK_FREQ, DEBUG_SCI_BAUDRATE, (SCI_CONFIG_WLEN_8|SCI_CONFIG_STOP_ONE|SCI_CONFIG_PAR_EVEN));
    SCI_disableLoopback(DEBUG_SCI_BASE);
    SCI_performSoftwareReset(DEBUG_SCI_BASE);
    SCI_setFIFOInterruptLevel(DEBUG_SCI_BASE, SCI_FIFO_TX0, SCI_FIFO_RX0);
    SCI_enableFIFO(DEBUG_SCI_BASE);
    SCI_enableModule(DEBUG_SCI_BASE);
}


#endif /* MODBUSSCI_H_ */
