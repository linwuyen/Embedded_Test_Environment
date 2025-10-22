/*
 * f021Bbox.c
 *
 *  Created on: Apr 16, 2024
 *      Author: Cody
 */
#include "f021Bbox.h"

typedef struct {
    uint32_t u32Address;
    uint16_t u16Words;
    uint16_t u16Chksum;
} ST_BLACKBOX;

#define BBOX_VAR(name)   {(uint32_t)&name, sizeof(name), 0}
#define END_OF_BBOX_VAR  {0, 0, 0}

ST_BLACKBOX sBBoxTable[] = {
                       BBOX_VAR(sDrv.f32A),
                       BBOX_VAR(sDrv.f32B),
                       BBOX_VAR(sDrv.f32C),
                       BBOX_VAR(sDrv.f32D),
                       BBOX_VAR(sDrv.f32E),
                       END_OF_BBOX_VAR
};

typedef enum {
    _NO_ACTION_FOR_BBOX        = (0x00000000),
    _BBOX_FREE,
    _BBOX_CALLBACK_SUCCESS,

    _RESERVED_FOR_BBOX         = (0x00010000<<0),

    _READ_BACK_FROM_BBOX       = (0x00010000<<1),
    _GET_THE_AMMOUNT_OF_BBOX_VAR,
    _IS_VALID_BBOX_VAR_IN_FLASH,

    _WRITE_BBOX_INTO_FLASH     = (0x00010000<<2),
    _INIT_FLASH_FOR_BBOX,
    _INIT_BANK0_FOR_BBOX,
    _ERASE_THE_FLASH_SPACE_FOR_BBOX,
    _WAIT_FOR_ERASING_BBOX_READY,
    _PROGRAM_BBOX_VAR_INTO_FLASH,
    _WAIT_FOR_PROGRAM_BBOX_READY,

    _MARK_ERROR_FOR_BBOX        = (0x80000000)
}FSM_BBOX;


typedef union {
    uint16_t all[8];
    struct {
        uint16_t chksum;
        uint16_t size;
        uint16_t data[4];
        uint32_t address;
    };
}DATA_128BITS;

typedef DATA_128BITS * HAL_128BITS;

typedef struct {
    FSM_BBOX fsm;
    FSM_BBOX stat;
    uint32_t u32Ammount;
    uint32_t u32Index;
    HAL_128BITS pFlashData;
    DATA_128BITS regTempData;
    uint32_t u32DelayCnt;
    Fapi_StatusType oReturnCheck;
}ST_BBOX;

ST_BBOX sBbox =  {
     .fsm = _READ_BACK_FROM_BBOX,
     .stat = _NO_ACTION_FOR_BBOX,
     .u32Ammount = 0,
     .u32DelayCnt = FAPI_DELAY_CNTS
};

uint16_t isBboxFree(void)
{
    return (_BBOX_FREE == sBbox.fsm);
}

uint16_t isCallbackReady(void)
{
    return ((sBbox.stat & (_BBOX_CALLBACK_SUCCESS)) == (_BBOX_CALLBACK_SUCCESS));
}

#ifdef _FLASH
#pragma SET_CODE_SECTION(".fapi_ram")
#endif //_FLASH

void runFlashStorage(void)
{

    EALLOW;
    switch(sBbox.fsm) {
    case _NO_ACTION_FOR_BBOX:
        break;

    case _READ_BACK_FROM_BBOX:
    case _GET_THE_AMMOUNT_OF_BBOX_VAR:
        if(0 != sBBoxTable[sBbox.u32Ammount].u32Address) sBbox.u32Ammount++;
        else {
            if(0 < sBbox.u32Ammount) {
                sBbox.u32Index = 0;
                sBbox.pFlashData = (HAL_128BITS)C28_STORE_ADDRESS;
                sBbox.fsm = _IS_VALID_BBOX_VAR_IN_FLASH;
            }
            else sBbox.fsm = _NO_ACTION_FOR_BBOX;
        }
        break;

    case _IS_VALID_BBOX_VAR_IN_FLASH:
        if(sBbox.u32Index < sBbox.u32Ammount) {
            uint16_t u16chksum = sBbox.pFlashData[sBbox.u32Index].chksum;
            for(uint16_t i = 1; i<8; i++) {
                u16chksum -= sBbox.pFlashData[sBbox.u32Index].all[i];
            }

            if((0 == u16chksum)&&(sBbox.pFlashData[sBbox.u32Index].size == sBBoxTable[sBbox.u32Index].u16Words)) {
                uint16_t *pu16 = (uint16_t *)sBBoxTable[sBbox.u32Index].u32Address;
                for(uint16_t i = 0; i<sBbox.pFlashData[sBbox.u32Index].size; i++) {
                    pu16[i] = sBbox.pFlashData[sBbox.u32Index].data[i];
                }

                sBbox.stat |= _BBOX_CALLBACK_SUCCESS;
            }
            sBbox.u32Index++;
        }
        else {
            sBbox.fsm = _BBOX_FREE;
        }
        break;


    case _WRITE_BBOX_INTO_FLASH:
    case _INIT_FLASH_FOR_BBOX:
        sBbox.oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, CPUCLK_FREQUENCY);
        if(Fapi_Status_Success == sBbox.oReturnCheck) {
            sBbox.fsm = _INIT_BANK0_FOR_BBOX;
        }
        break;

    case _INIT_BANK0_FOR_BBOX:
        sBbox.oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
        if(Fapi_Status_Success == sBbox.oReturnCheck) {
            sBbox.fsm = _ERASE_THE_FLASH_SPACE_FOR_BBOX;
        }
        break;

    case _ERASE_THE_FLASH_SPACE_FOR_BBOX:
        sBbox.oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, (uint32 *)sBbox.pFlashData);
        sBbox.fsm = _WAIT_FOR_ERASING_BBOX_READY;

    case _WAIT_FOR_ERASING_BBOX_READY:
        sBbox.oReturnCheck = Fapi_checkFsmForReady();
        if(Fapi_Status_FsmReady == sBbox.oReturnCheck) {
            sBbox.u32Index = 0;
            sBbox.fsm = _PROGRAM_BBOX_VAR_INTO_FLASH;
        }
        break;

    case _PROGRAM_BBOX_VAR_INTO_FLASH:
        if(sBbox.u32Index < sBbox.u32Ammount) {
            uint16_t *pu16 = (uint16_t *)sBBoxTable[sBbox.u32Index].u32Address;
            sBbox.regTempData.address = sBBoxTable[sBbox.u32Index].u32Address;
            sBbox.regTempData.size = sBBoxTable[sBbox.u32Index].u16Words;
            sBbox.regTempData.chksum = sBbox.regTempData.size + sBbox.regTempData.address;
            for(uint16_t i = 0; i<4; i++) {
                if(i<sBBoxTable[sBbox.u32Index].u16Words) {
                    sBbox.regTempData.data[i] = pu16[i];
                    sBbox.regTempData.chksum += pu16[i];
                }
                else {
                    sBbox.regTempData.data[i] = 0;
                }
            }

            Fapi_issueProgrammingCommand((uint32 *)&sBbox.pFlashData[sBbox.u32Index], (uint16 *)sBbox.regTempData.all, ALIGN_WORD_ADDRESS_BY,
                                         0, 0, Fapi_AutoEccGeneration);
            sBbox.fsm = _WAIT_FOR_PROGRAM_BBOX_READY;
        }
        else {
            sBbox.fsm = _BBOX_FREE;
        }
        break;
    case _WAIT_FOR_PROGRAM_BBOX_READY:
        sBbox.oReturnCheck = Fapi_checkFsmForReady();
        if(Fapi_Status_FsmReady == sBbox.oReturnCheck) {
            sBbox.u32Index++;
            sBbox.fsm = _PROGRAM_BBOX_VAR_INTO_FLASH;
        }
    default:
        break;
    }

    EDIS;
}

#ifdef _FLASH
#pragma SET_CODE_SECTION()
#endif //_FLASH
//
// End of File
//
