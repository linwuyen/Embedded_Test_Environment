/*
 * timetask.c
 *
 *  Created on: May 17, 2024
 *      Author: User
 */

#include "common.h"


ST_PARAMS sParams;
uint32_t u32Eeprom[256] = {0x00000000,0x11112222,0x33334444,0x56565656,0x77888877};
uint32_t u32Test[5] = {0x00000000,0x11223344,0x55667788,0x12345678,0x98765432};

ST_SPI_MASTER sSpiM =
{
                       .u32Fsm = _SEND_SPI_PACK,
                       .u16Pop = 0,
                       .u16Push = 0,
                       .u32Timeout = 0,
                       .u32TimeStamp = 0,
                       .u32TimeMark = 0,
                       .u32ErrCnts = 0,
                       .u32OkCnts = 0
};

ST_SPI_SLAVE sSpiS = {
                       .u32Fsm = _IS_THRER_A_SPI_PACK,
                       .u32Timeout = 0,
                       .u32TimeStamp = 0,
                       .u32TimeMark = 0,
                       .u32ErrCnts = 0,
                       .p32Data = (uint32_t*)&sParams,
                       .p32Eeprom = u32Eeprom,
                       .s16Rsize = 4

};

void task500msec(void * s){

    updateFANCTRL5Duty();
    //40us
    Temperature();

}


void isThereNewDataInRAM(void){

    static uint32_t u32Temp = 0;
    if(sSpiS.u32CntWriteEeprom != u32Temp) {
        rstWriteUserBoxTimeStamp();
        u32Temp = sSpiS.u32CntWriteEeprom;
    }
}

void task1msec(void * s){

    static uint16_t u16ToggleDAC = 4095U;

    isThereNewDataInRAM();

    if(4095U == u16ToggleDAC) {
        DAC_setShadowValue(myDACB_BASE, 0U);
        u16ToggleDAC = 0;
    }
    else {
        DAC_setShadowValue(myDACB_BASE, 4095U);
        u16ToggleDAC = 4095U;
    }
    Update_IO();
}


void asapTask(void * s){

    excSpiSlave(&sSpiS);
    runUserBox();
    SPIA_Control();

}

ST_TIMETASK time_task[] = {
        {task1msec,           0,   T_1MS},
        {task500msec,          0,   T_500MS},
        {asapTask,            0,   0},
        END_OF_TASK
};

void pollTimeTask(void){

    scanTimeTask(time_task, (void *)0);

}
