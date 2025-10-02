/*
 * Modbus_Ctrl.c
 *
 *  Created on: Jul 22, 2025
 *      Author: roger_lin
 */

#include "Modbus_Ctrl.h"

ST_DRV sDrv;
ModbusController Conn;


void Slave_judge(){

    if (mbcomm_s.slaveid == 0 && regMbusData.u16SLAveid != 0)
    {
        GPIO_writePin(57, 1);
        mbcomm_s.slaveid = regMbusData.u16SLAveid;
    }
}

uint32_t u32_currentTime;
uint32_t u32_elapsedTime;

uint32_t u32_wait_start_time;
uint32_t u32_wait_timer_flag;
uint16_t MBUS_WAITmC;

void AutoAddress_FSM(ModbusController *p)
{
    CMDPACK cmd;

    switch(p->STATE)
    {
        case ADDR_STATE_IDLE:
            break;

        case ADDR_STATE_START:
            p->next_id_to_assign = 1;
            p->total_discovered = 0;
            p->STATE = ADDR_STATE_BROADCAST_WRITE;
            break;

        case ADDR_STATE_BROADCAST_WRITE:
            cmd = (CMDPACK){
                .slave = 0,
                .function = MB_PRESET_SINGLE_REGISTERm,
                .address = SLAVE_REG_ADDR,
                .points = 1,
                .reg = &p->next_id_to_assign};

            pushCmdPack(&cmd);


            p->STATE = ADDR_STATE_SEND_VERIFY_READ;
            break;


        case ADDR_STATE_SEND_VERIFY_READ:
            mbcomm_m.state = MBUS_WAITm;
            cmd = (CMDPACK){
                .slave = p->next_id_to_assign,
                .function = MB_READ_HOLDING_REGISTERSm,
                .address = SLAVE_REG_ADDR,
                .points = 1,
                .reg = &p->next_id_to_assign};

            pushCmdPack(&cmd);


            p->STATE = ADDR_STATE_WAIT_FOR_RESPONSE;
            break;

//
        case ADDR_STATE_WAIT_FOR_RESPONSE:


            if (mbcomm_m.state == MBUS_SUCCESSm)
            {
                p->STATE = ADDR_STATE_SUCCESS;
            }

            else if (mbcomm_m.state == MBUS_FAILm )
            {
                p->STATE = ADDR_STATE_ERROR;
            }

            else if (mbcomm_m.state == MBUS_WAITm)
            {

                if (u32_wait_timer_flag == 0)
                {
                    u32_wait_start_time = SW_TIMER - U32_COUNTER;
                    u32_wait_timer_flag = 1;
                }

                u32_currentTime = SW_TIMER - U32_COUNTER;
                u32_elapsedTime = u32_currentTime - u32_wait_start_time;


                if (u32_elapsedTime >= T_200MS)
                {
                    //reset
                    mbcomm_m.evstep =_INIT_SCI_GPIOm;
                    mbcomm_m.evHoldReg= _SEND_SLAVEID_AND_FUNCIDm;
                    mbcomm_m.state = MBUS_SUCCESSm;
                    MBUS_WAITmC++;

                    u32_wait_timer_flag = 0;
                    p->STATE = ADDR_STATE_FINISH;
                }
            }
            break;
//
        case ADDR_STATE_SUCCESS:

            if (p->next_id_to_assign > MAX_POSSIBLE_SLAVES) {
                p->STATE = ADDR_STATE_FINISH;
            } else {
                p->total_discovered++;
                p->next_id_to_assign++;
                p->STATE = ADDR_STATE_BROADCAST_WRITE;
            }
            break;
//
        case ADDR_STATE_FINISH:
            break;
//
        case ADDR_STATE_ERROR:
            break;

        case TEST:
//            p->Package.slave =     p->TestPack.slave;
//            p->Package.function =  p->TestPack.function;
//            p->Package.address =   p->TestPack.address;
//            p->Package.points =    p->TestPack.points ;
//            p->Package.reg = (Uint16*) &regMbusData.u16MbusData[6];
//            pushCmdPack(&(p->Package));
//            p->STATE = ADDR_STATE_IDLE;
            break;
    }
}

void Run_Modbus_FSM()
{
        ModbusController *p = &Conn;

        if (p->AM3352 == 0)
        {
            p->MODE = MODE_SLAVE;
            exeModbusSlave((SCI_MODBUS*) &mbcomm_s);
            Slave_judge();
        }
        else
        {
            p->MODE = MODE_MASTER;
            exeModbusMaster((SCI_MODBUSm*) &mbcomm_m);
            AutoAddress_FSM(&Conn);
        }
}
