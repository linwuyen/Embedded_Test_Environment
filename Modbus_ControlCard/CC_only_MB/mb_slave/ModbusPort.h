/*
 * ModbusPort.h
 *
 *  Created on: Mar 4, 2024
 *      Author: cody_chen
 */

#ifndef MB_SLAVE_MODBUSPORT_H_
#define MB_SLAVE_MODBUSPORT_H_

#include "Modbus_Ctrl.h"
#include "ModbusSlave.h"



extern uint16_t setMbusSendByte(SCI_MODBUS *mbus, uint16_t data);
extern uint16_t getMbusReadByte(SCI_MODBUS *mbus);
extern int16_t getMbusRxSize(SCI_MODBUS *mbus);
extern int16_t pollModbusPort(SCI_MODBUS *mbus);

#endif /* MB_SLAVE_MODBUSPORT_H_ */
