/*
 *  File Name: linkEeprom.h
 *
 *  Created on: 8/26/2025
 *  Author: POWER-532A86
 */

typedef struct {
    uint16_t size;
    void *ptr;
} EE_REG;
typedef EE_REG * HAL_EEREG;

#define _BLK16_USERREGS	7
uint16_t blkUserRegs[_BLK16_USERREGS];
const EE_REG regUserRegs[4] = {
              {2, (void*)&},
              {2, (void*)&},
              {2, (void*)&},
              {1, (void*)&blkUserRegs[6]}};
#define _TABLE_USERREGS	4

#define _BLK16_BOARDREGS	33
uint16_t blkBoardRegs[_BLK16_BOARDREGS];
const EE_REG regBoardRegs[9] = {
              {4, (void*)&},
              {4, (void*)&},
              {4, (void*)&},
              {4, (void*)&},
              {4, (void*)&},
              {4, (void*)&},
              {4, (void*)&},
              {4, (void*)&},
              {1, (void*)&blkBoardRegs[32]}};
#define _TABLE_BOARDREGS	9

