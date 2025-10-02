/*
 * Modbus_Ctrl.h
 *
 *  Created on: Jul 22, 2025
 *      Author: roger_lin
 */

#ifndef MODBUS_CTRL_H_
#define MODBUS_CTRL_H_

#include "driverlib.h"
#include "device.h"
#include "board.h"
//#include "c2000ware_libraries.h"

#include "timetask.h"
#include "ModbusMaster.h"
#include "ModbusSlave.h"


#define SLAVE_REG_ADDR            1000
#define MAX_POSSIBLE_SLAVES         10

#define PAR_ENA          0
#define PAR_DIS          1

#define RE               0  //485 READ Mode
#define DE               1  //485 WIRTE Mode

typedef enum
{
    MODE_MASTER = 0,
    MODE_SLAVE,
} Modbus_Mode;

typedef enum {
    ADDR_STATE_IDLE = 0,
    ADDR_STATE_START,
    ADDR_STATE_BROADCAST_WRITE,
    ADDR_STATE_SEND_VERIFY_READ,
    ADDR_STATE_WAIT_FOR_RESPONSE,
    ADDR_STATE_SUCCESS,
    ADDR_STATE_FINISH,
    ADDR_STATE_ERROR,
    TEST,
} AutoAddress_State;


typedef struct
{
    Modbus_Mode               MODE;
    AutoAddress_State        STATE;
    CMDPACK                Package;
    CMDPACK               TestPack;
    uint16_t                AM3352;
    Uint16       next_id_to_assign;
    uint16_t      total_discovered;
} ModbusController;


extern void Run_Modbus_FSM(void);

#endif /* MODBUS_CTRL_H_ */
