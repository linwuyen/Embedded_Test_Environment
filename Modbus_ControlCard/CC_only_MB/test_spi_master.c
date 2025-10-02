/*
 * test_spi_master.c
 *
 *  Created on: May 20, 2024
 *      Author: User
 */

#include "Global_VariableDefs.h"
#include "SPI_sParams.h"
ST_PARAMS sParams;
ST_PARAMS sParamsShadow;


ST_SPI_MASTER sSpiM = {
                       .u32Fsm = _SEND_SPI_PACK,
                       .u16Pop = 0,
                       .u16Push = 0,
                       .u32Timeout = 0,
                       .u32TimeStamp = 0,
                       .u32TimeMark = 0,
                       .u32ErrCnts = 0,
                       .u32OkCnts = 0
};



void scanSpiAction(void)
{
    if(isSpiPackFiFoFree(&sSpiM)) {
        for(ID_CMD u16Action = _ID_DIN; u16Action!=_END_OF_IDCMD; u16Action++) {

            switch(u16Action) {
            case _ID_DIN:
            case _ID_TEMP_C:
            case _ID_HEARTBEAT:
                pushSpiRpack(u16Action, (uint32_t*)&sParams, &sSpiM);
                break;

            case _ID_DOUT:
            case _ID_FAN_CTRL:
            case _ID_IREF_P:
            case _ID_IREF_N:
                if(((uint32_t*)&sParams)[u16Action] != ((uint32_t*)&sParamsShadow)[u16Action]) {
                    pushSpiWpack(u16Action, (uint32_t*)&sParams, &sSpiM);
                    ((uint32_t*)&sParamsShadow)[u16Action] = ((uint32_t*)&sParams)[u16Action];
                    return;
                }
                break;

            default:
                break;
            }
        }
    }
}


void popSpiPack(void)
{
    popSpiWdata(&sSpiM);

}


