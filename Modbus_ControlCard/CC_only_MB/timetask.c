/*
 * timetask.c
 *
 *  Created on: May 17, 2024
 *      Author: User
 */
#include "common.h"

ST_TIMER_MEAS t1 ={0};
ST_TIMER_MEAS t2 ={0};

void task500msec(void * s)
{
    measTimerLength(&t1);
    GPIO_togglePin(myBoardLED0_GPIO);
}


void task200msec(void * s)
{
    measTimerLength(&t2);
    GPIO_togglePin(myBoardLED0_GPIO);
//    if(0 == u16Action){
//        u16Action = u16Command;
//        u16Command++;
//        u32Test[0] = u32Test[1];
//        u32Test[1] = u32Test[2];
//        u32Test[2] = u32Test[3];
//        u32Test[3] = u32Test[4];
//        u32Test[4] = u32Test[0];
//        if(5 == u16Command) u16Command = 1;
//    }
}

void task1msec(void * s)
{
//    GPIO_togglePin(PAR);
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
