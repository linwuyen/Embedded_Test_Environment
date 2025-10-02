/*
 * timetask.c
 *
 *  Created on: May 17, 2024
 *      Author: User
 */

#include "Modbus_Ctrl.h"
#include "cTimeMeas.h"


void task500msec(void * s)
{
    GPIO_togglePin(myBoardLED0_GPIO);
}


void task200msec(void * s)
{
    GPIO_togglePin(myBoardLED1_GPIO);
}


void task1msec(void * s)
{

}


void asapTask(void * s)
{

    Modbus_FSM();
}

ST_TIMETASK time_task[] = {
        {task1msec,           0,   T_1MS},
        {task200msec,          0,   T_200MS},
        {task500msec,          0,   T_500MS},
        {asapTask,            0,   0},
        END_OF_TASK
};

void pollTimeTask(void)
{
    scanTimeTask(time_task, (void *)0);
}
