/*
 * f021Bbox.c
 *
 *  Created on: Apr 16, 2024
 *      Author: Cody
 */
#include "f021Bbox.h"
#include "spi_slave.h"

typedef enum {
    _NO_ACTION_FOR_UBOX           = (0x00000000),
    _UBOX_CALLBACK_SUCCESS,

    _RESERVED_FOR_UBOX            = (0x00010000<<0),

    _READ_BACK_FROM_FLASH_TO_RAM  = (0x00010000<<1),
    _WAIT_FOR_WRITE_UBOX,

    _WRITE_UBOX_INTO_FLASH        = (0x00010000<<2),
    _INIT_FLASH_FOR_UBOX,
    _INIT_BANK0_FOR_UBOX,
    _ERASE_THE_FLASH_SPACE_FOR_UBOX,
    _WAIT_FOR_ERASING_UBOX_READY,
    _PROGRAM_UBOX_VAR_INTO_FLASH,
    _WAIT_FOR_PROGRAM_UBOX_READY,

    _MARK_ERROR_FOR_UBOX          = (0x80000000)
}FSM_UBOX;


typedef struct {
    FSM_UBOX fsm;
    uint32_t u32Ammount;
    uint32_t u32Index;
    uint32_t *pFlash;
    uint32_t *pEeprom;
    Fapi_StatusType oReturnCheck;

    uint32_t u32TimeMark;
    uint32_t u32TimeStamp;
    uint32_t u32Timeout;
    uint32_t u32TimeoutTarget;

}ST_USERBOX;

extern uint32_t u32Eeprom[256];

ST_USERBOX sUbox =  {
     .pFlash = (uint32_t*)C28_USER_BOX,
     .pEeprom = (uint32_t*)u32Eeprom,
     .u32Ammount = sizeof(u32Eeprom)/sizeof(uint32_t),
     .u32Index = 0,
     .u32TimeoutTarget = T_1MS,
     .fsm = _READ_BACK_FROM_FLASH_TO_RAM
};

void rstWriteUserBoxTimeStamp(void)
{
    sUbox.u32TimeoutTarget += T_1MS;
    sUbox.u32TimeMark = sUbox.u32TimeStamp = U32_UPCNTS;
}

#ifdef _FLASH
#pragma SET_CODE_SECTION(".fapi_ram")
#endif //_FLASH

void runUserBox(void)
{

    EALLOW;
    switch(sUbox.fsm) {
    case _NO_ACTION_FOR_UBOX:
        break;

    case _READ_BACK_FROM_FLASH_TO_RAM:
        if(sUbox.u32Index < sUbox.u32Ammount) {
            sUbox.pEeprom[sUbox.u32Index] = sUbox.pFlash[sUbox.u32Index];
            sUbox.u32Index++;
        }
        else {
            sUbox.fsm = _WAIT_FOR_WRITE_UBOX;
        }
        break;

    case _WAIT_FOR_WRITE_UBOX:
        if(0 != sUbox.u32TimeMark) {
            sUbox.u32TimeStamp = U32_UPCNTS;

            if(sUbox.u32TimeMark > sUbox.u32TimeStamp) {
                sUbox.u32Timeout = sUbox.u32TimeStamp + SW_TIMER - sUbox.u32TimeMark;
            }
            else {
                sUbox.u32Timeout = sUbox.u32TimeStamp - sUbox.u32TimeMark;
            }

            if(sUbox.u32Timeout >= sUbox.u32TimeoutTarget) {
                sUbox.u32TimeMark = 0;
                sUbox.u32TimeoutTarget = T_1MS;
                sUbox.fsm = _WRITE_UBOX_INTO_FLASH;
            }
        }

        break;

    case _WRITE_UBOX_INTO_FLASH:
    case _INIT_FLASH_FOR_UBOX:
        sUbox.oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, CPUCLK_FREQUENCY);
        if(Fapi_Status_Success == sUbox.oReturnCheck) {
            sUbox.fsm = _INIT_BANK0_FOR_UBOX;
        }
        break;

    case _INIT_BANK0_FOR_UBOX:
        sUbox.oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
        if(Fapi_Status_Success == sUbox.oReturnCheck) {
            sUbox.fsm = _ERASE_THE_FLASH_SPACE_FOR_UBOX;
        }
        break;

    case _ERASE_THE_FLASH_SPACE_FOR_UBOX:
        sUbox.oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, (uint32 *)sUbox.pFlash);
        sUbox.fsm = _WAIT_FOR_ERASING_UBOX_READY;

    case _WAIT_FOR_ERASING_UBOX_READY:
        sUbox.oReturnCheck = Fapi_checkFsmForReady();
        if(Fapi_Status_FsmReady == sUbox.oReturnCheck) {
            sUbox.u32Index = 0;
            sUbox.fsm = _PROGRAM_UBOX_VAR_INTO_FLASH;
        }
        break;

    case _PROGRAM_UBOX_VAR_INTO_FLASH:
        if(sUbox.u32Index < sUbox.u32Ammount) {
            Fapi_issueProgrammingCommand(&sUbox.pFlash[sUbox.u32Index], (uint16_t*)&sUbox.pEeprom[sUbox.u32Index], ALIGN_WORD_ADDRESS_BY,
                                             0, 0, Fapi_AutoEccGeneration);
            sUbox.fsm = _WAIT_FOR_PROGRAM_UBOX_READY;
        }
        else {
            sUbox.fsm = _WAIT_FOR_WRITE_UBOX;
        }
        break;
    case _WAIT_FOR_PROGRAM_UBOX_READY:
        sUbox.oReturnCheck = Fapi_checkFsmForReady();
        if(Fapi_Status_FsmReady == sUbox.oReturnCheck) {
            sUbox.u32Index+=(ALIGN_WORD_ADDRESS_BY/4);
            sUbox.fsm = _PROGRAM_UBOX_VAR_INTO_FLASH;
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
