/*
 * timetask.c
 *
 *  Created on: May 17, 2024
 *      Author: User
 */
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "timetask.h"



void task500msec(void * s)
{

}


void task10msec(void * s)
{
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


void isThereNewDataInRAM(void)
{

}


void task1msec(void * s)
{

//    GPIO_togglePin(PAR);

}


void asapTask(void * s)
{

}

ST_TIMETASK time_task[] = {
        {task1msec,           0,   T_1MS},
        {task10msec,          0,   T_10MS},
        {task500msec,          0,   T_500MS},
        {asapTask,            0,   0},
        END_OF_TASK
};

void pollTimeTask(void)
{
    scanTimeTask(time_task, (void *)0);
}
