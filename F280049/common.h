/*
 * common.h
 *
 *  Created on: Mar 1, 2024
 *      Author: User
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "HwConfig.h"
#include "c28param.h"
#include "flashapi/f021api.h"
#include "flashapi/f021Bbox.h"
#include "timetask.h"

#include "SPWM_Ctrl.h"
#include "C28ClaVars.h"
#include "spi_slave.h"
#include "spi_master.h"

#include "flashapi/f021UserBox.h"

// New Integrated Modules
#include "ADC_Module/adc_control.h"
#include "CLA_Module/cla_tasks.h"
#include "Diagnostic/diag_crystal.h"

// DDS Module and Manager
#include "DDS_Module/DDS.h"
#include "WaveGen_Manager/WaveGen_Manager.h"


#endif /* COMMON_H_ */
