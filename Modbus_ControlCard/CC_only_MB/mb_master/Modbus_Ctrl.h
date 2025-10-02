/*
 * Modbus_Ctrl.h
 *
 *  Created on: Jul 22, 2025
 *      Author: roger_lin
 */

#ifndef MODBUS_CTRL_H_
#define MODBUS_CTRL_H_

#include "board.h"

#include "timetask.h"
#include "ModbusMaster.h"
#include "ModbusSlave.h"


#define SLAVE_ID_ADDR         17      // Modbus Register
#define RAM_SIZE              8
#define MAX_SLAVES            5
#define MAX_IO_COMPARE_RETRIES  5
#define APP_IO_POLLING_DELAY_MS 360360    // Polling delay, calculated for an approx. 2.775us tick time

#define PAR_ENA          0
#define PAR_DIS          1

#define RE               0  //485 READ Mode
#define DE               1  //485 WIRTE Mode

//------------------------------------------
typedef enum
{
    MODE_MASTER = 0,
    MODE_SLAVE,
} Modbus_Mode;


typedef enum
{
    ASSIGN_INITIAL,
    ASSIGN_IDLE,
    ASSIGN_FINALIZE,
    ASSIGN_RESET
} ASSIGN_FSM;


typedef enum
{
    ID_INITIAL = 0,
    ID_IDLE,
    ID_BROADCAST_WRITE,
    ID_POST_BROADCAST,
    ID_SEND_VERIFY_READ,
    ID_WAIT_FOR_RESPONSE,
    ID_FINISH,
    ID_TIMEOUT,
    ID_RESET,
} AUTOID_FSM;


typedef enum {
    ID_RESULT_IN_PROGRESS,
    ID_RESULT_SUCCESS,
    ID_RESULT_TIMEOUT
} RESULT_FSM;


typedef enum{
    APP_IO_IDLE,
    MASTER_PIN_SWITCH,
    APP_IO_READ,
    APP_IO_SAVE,
    APP_IO_COMPARE,
    APP_IO_EXECUTE,
    APP_IO_ERROR,
    APP_IO_POLL_DELAY, // Add a delay state for polling
    APP_IO_RESET
} AppIOFSM;


typedef struct
{
    AUTOID_FSM      state;
    RESULT_FSM      result;
    Uint16         nextId;
    Uint16        totalIds;
    Uint16        discoveredIds[MAX_SLAVES];

} ModbusAutoIdManager;


typedef struct
{
    AppIOFSM             state;
    CMDPACK         cmdPackage;
    uint16_t*        pRxBuffer;
    uint16_t         dataIndex;

    uint16_t        slave1Data[RAM_SIZE];
    uint16_t        writeData;
    uint16_t      io_mismatch_mask;
    uint16_t      pollDelayCounter; // Counter for polling delay
} ModbusAppIOManager;



typedef struct
{
    Modbus_Mode            mode;
    ASSIGN_FSM      assignState;
    ModbusAutoIdManager autoId;
    ModbusAppIOManager  appIO;


    uint16_t            Temp_buffer;
    uint16_t                temp;
    float         Temp_Sensor_SLAVE;
    uint16_t     Temp_Sensor;
    uint16_t is_temp_ok;
} ModbusController;


extern ModbusController MCTRL;
extern void Modbus_FSM(void);

#endif /* MODBUS_CTRL_H_ */
