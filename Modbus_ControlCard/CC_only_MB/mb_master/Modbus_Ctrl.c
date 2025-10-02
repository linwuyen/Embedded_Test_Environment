/*
 * Modbus_Ctrl.c
 *
 *  Created on: Jul 22, 2025
 *      Author: roger_lin
 */
#include "Global_VariableDefs.h"
#include "Modbus_Ctrl.h"

ST_DRV sDrv;
ModbusController Conn;

#define PAR_ENA_0         1// {GpioDataRegs.GPBCLEAR.bit.GPIO53 = 1;}
#define PAR_ENA_1         2// {GpioDataRegs.GPBSET.bit.GPIO53 = 1;}

void Slave_judge(){

//    if (mbcomm_s.slaveid == 0 && regMbusData.u16SLAveid != 0)
//    {
//        GPIO_writePin(57, 1);
////        mbcomm_s.slaveid = regMbusData.u16SLAveid;
//    }
}

uint32_t u32_currentTime;
uint32_t u32_elapsedTime;

uint32_t u32_wait_start_time;
uint32_t u32_wait_timer_flag;
uint16_t MBUS_WAITmC;

void AutoID_FSM(ModbusController *p)
{
    switch (p->autoId.state)
    {
        case ID_INITIAL:
            p->autoId.nextId = 1;
            p->autoId.totalIds = 0;

            PAR_ENA_0;

            p->autoId.state = ID_BROADCAST_WRITE;
            break;

        case ID_IDLE:
            break;

        case ID_BROADCAST_WRITE:
            {
                CMDPACK cmd = {
                    .slave    = 0,
                    .function = MB_PRESET_SINGLE_REGISTERm,
                    .address  = SLAVE_ID_ADDR,
                    .points   = 1,
                    .reg      = &p->autoId.nextId
                };
                pushCmdPack(&cmd);
                p->autoId.state = ID_POST_BROADCAST;
            }
            break;

        case ID_POST_BROADCAST:

            if (mbcomm_m.evstep == _POP_COMMAND_OUTm)
            {
                p->autoId.state = ID_SEND_VERIFY_READ;
            }
            break;

        case ID_SEND_VERIFY_READ:
            {
                CMDPACK cmd = {
                    .slave    = p->autoId.nextId,
                    .function = MB_READ_HOLDING_REGISTERSm,
                    .address  = SLAVE_ID_ADDR,
                    .points   = 1,
                    .reg      = &p->autoId.discoveredIds[p->autoId.totalIds]
                };
                pushCmdPack(&cmd);

                mbcomm_m.state = MBUS_WAITm;

                p->autoId.state = ID_WAIT_FOR_RESPONSE;
            }
            break;

        case ID_WAIT_FOR_RESPONSE:
            if (mbcomm_m.state == MBUS_SUCCESSm)
            {

                mbcomm_m.state = MBUS_WAITm;
                p->autoId.totalIds++;
                p->autoId.nextId++;
                if (p->autoId.nextId > MAX_SLAVES) {
                    p->autoId.state = ID_FINISH;
                } else {
                    p->autoId.state = ID_BROADCAST_WRITE;
                }
            }
            else if (mbcomm_m.state == MBUS_FAILm)
            {
                p->autoId.state = ID_FINISH;
//                p->autoId.state = ID_BROADCAST_WRITE;
            }

            break;

        case ID_FINISH:
            p->autoId.result = ID_RESULT_SUCCESS;
           p->autoId.state = ID_IDLE;
            break;

//        case ID_TIMEOUT:
//            p->autoId.result = ID_RESULT_SUCCESS;
////            mbcomm_m.state = MBUS_SUCCESSm;
//            p->autoId.state = ID_IDLE;
//            break;

        case ID_RESET:
            p->autoId.nextId = 1;
            p->autoId.totalIds = 0;
            p->autoId.result = ID_RESULT_IN_PROGRESS;
            mbcomm_m.evstep = _INIT_SCI_GPIOm;
            mbcomm_m.evHoldReg = _SEND_SLAVEID_AND_FUNCIDm;
            p->autoId.state = ID_INITIAL;
            break;
    }
}

//void Remote_IO(ModbusController *p)
//{
//    if (p->autoId.result == ID_RESULT_SUCCESS)
//    {
//        switch(p->appIO.state)
//        {
//            case APP_IO_IDLE:
//                break;
//
//            case MASTER_PIN_SWITCH:
//
//                if (p->autoId.totalIds > 0) {
//
//                    write_single_register(p, p->autoId.discoveredIds[0], _muMASterpin);
//                }
//                p->appIO.state = APP_IO_IDLE;
//                break;
//
//
//            case APP_IO_READ:
//
//                if (p->autoId.totalIds > 0) {
//
//                     read_registers(p, p->autoId.discoveredIds[0], SLAVE_ID_ADDR, REMOTE_IO_REGISTER_COUNT);
//                }
//
////                p->appIO.state = APP_IO_POLL_DELAY;
//                p->appIO.state = APP_IO_SAVE;
//
//                break;
//
//
//            case APP_IO_SAVE:
//                if (mbcomm_m.state == MBUS_SUCCESSm)
//                {
//
//                    mbcomm_m.state = MBUS_WAITm;
//
//
//                    uint16_t byte_count = mbcomm_m.RxBuffer[2];
//                    uint16_t word_count = byte_count / 2;
//
//                    if (word_count > RAM_SIZE) {
//                        word_count = RAM_SIZE;
//                    }
//
//                    for (uint16_t i = 0; i < word_count; i++)
//                    {
//
//                        uint16_t high_byte = mbcomm_m.RxBuffer[3 + (i * 2)];
//                        uint16_t low_byte  = mbcomm_m.RxBuffer[4 + (i * 2)];
//                        p->appIO.slave1Data[i] = (high_byte << 8) | low_byte;
//                    }
//                    p->appIO.state = APP_IO_COMPARE;
//                }
//                else {
//                    mbcomm_m.state = MBUS_SUCCESSm;
//                }
//
//                break;
//
//            case APP_IO_COMPARE:
//            {
//                  uint16_t local_io_status  = sParams.sDin.u16Data[0];
//                  uint16_t remote_io_status = p->appIO.slave1Data[SLAVE_REG_INDEX_IO_STATUS];
//                  uint16_t remote_raw_temp  = p->appIO.slave1Data[SLAVE_REG_INDEX_TEMPERATURE];
//
//                  uint16_t temp_buffer = remote_raw_temp & 0x0FFF;
//                  float temp_sensor_slave = (float)temp_buffer * ADC_TO_TEMP_C;
//                  p->Temp_Sensor = temp_sensor_slave;
//
//                  p->is_temp_ok = (  p->Temp_Sensor <= DCAC_OTP_LEVEL );
//
//
//                  p->temp = p->is_temp_ok;
//
//                  sGB.fgError1.bit.DCAC_OTP_SW = !p->is_temp_ok;
//
//                  p->appIO.io_mismatch_mask = local_io_status ^ remote_io_status;
//
//                  // The state transition logic now correctly checks if IOs match AND temperature is OK.
//                  if (p->appIO.io_mismatch_mask == 0 && p->is_temp_ok)
//                  {
//                      p->appIO.state = APP_IO_EXECUTE;
//                  }
//                  else
//                  {
//                      p->appIO.state = APP_IO_ERROR;
//                  }
//                  break;
//            }
//            case APP_IO_EXECUTE:
//
//                //OPEN RELAY
//                sGB.OUTPUT_FLAGS.bit.OUT_ON = 1;
//                sGB.OUTPUT_FLAGS.bit.REMOTE = 1;
//
//                // Go to delay state to wait for the next poll.
//                p->appIO.pollDelayCounter = APP_IO_POLLING_DELAY_MS;
////                p->appIO.state = APP_IO_POLL_DELAY;
//                p->appIO.state = APP_IO_READ;
//                break;
//
//            case APP_IO_ERROR:
//
//                //CLOSE RELAY
//                sGB.OUTPUT_FLAGS.bit.OUT_ON = 0;
//                sGB.OUTPUT_FLAGS.bit.REMOTE = 0;
//
//                // Go to delay state to wait for the next poll.
//                p->appIO.pollDelayCounter = APP_IO_POLLING_DELAY_MS;
////                p->appIO.state = APP_IO_POLL_DELAY;
//                break;
//
//            case APP_IO_POLL_DELAY:
//                // Wait until the delay counter reaches zero.
////                if (mbcomm_m.state == MBUS_FAILm)
//                    if (mbcomm_m.state == MBUS_SUCCESSm)
//                {
//                    p->appIO.state = APP_IO_READ;
//                }
//                break;
//
//        }
//    }
//}

void Modbus_FSM()
{
    static uint16_t previousState = 0;

    if (sGB.RD_MODE.bit.Master == 1 && previousState == 0)
    {
        MCTRL.autoId.state = ID_RESET;
    }

    previousState = sGB.RD_MODE.bit.Master;

    if (sGB.RD_MODE.bit.Master == 0)
    {
        MCTRL.mode = MODE_SLAVE;
//        sParams.sDout.o_stat.b18_SPIA_State = 0;
        exeModbusSlave((SCI_MODBUS*) &mbcomm_s);
        Slave_judge(&MCTRL);
    }
    else
    {
        MCTRL.mode = MODE_MASTER;
//        sParams.sDout.o_stat.b18_SPIA_State = 1;
        exeModbusMaster((SCI_MODBUSm*) &mbcomm_m);
        AutoID_FSM(&MCTRL);
        Remote_IO(&MCTRL);
        }
}
