//###########################################################################
//
// FILE:    Global_VariableDefs.h
//
// TITLE:
//
//###########################################################################
// $Create Date: 20210819
//###########################################################################

#include <stdint.h>




#ifdef DEBUG_ON
#define C28_SCI_TX() scia_TXwrite_char(SCI_Module_Variables.Debug_Msg , strlen(SCI_Module_Variables.Debug_Msg)); //4,575,499 cycles
#define C28_PRINTF(format, data) sprintf(SCI_Module_Variables.Debug_Msg, format, data), C28_SCI_TX();
#else
#define C28_PRINTF(format, data)
#endif

#define CPUTIMERx(Tstamp, Timeout, TimerNo) (Calc_Elapsed_Time(Tstamp, CpuTimer##TimerNo.InterruptCount ) > Timeout)
#define TMR0(Tstamp, Timeout)               CPUTIMERx(Tstamp, Timeout, 0)
#define TMR1(Tstamp, Timeout)               CPUTIMERx(Tstamp, Timeout, 1)
#define TMR2(Tstamp, Timeout)               CPUTIMERx(Tstamp, Timeout, 2)

#define FG_GET(x, src)   ((src & (x)) == (x))
#define FG_GETn(x, src)  (!FG_GET(x, src))
#define FG_AND(x, src)   (src & (x))
#define FG_SET(x, src)   {src |= (x);}
#define FG_RST(x, src)   {src &= ~(x);}
#define FG_SWTO(x, src)  {src = (x);}

/////////////////////////////////////////////////////////////////////
/*************機種*************/

#define ASR5000

#define ASR_5075   0
#define ASR_5150   1

/////////////////////////////////////////////////////////////////////
/*************版本*************/
#define  HW_Version                 0x0001  //機器硬體版本

#ifdef   ASR5000
#define  DSP_FW_Version_Code        0x8A90  //DSP韌體版本
#endif

#define  CALI_DATA_VER              0x0003  //校正參數版本


///////////////////////////////////////////////////////////////////////
//#include "F28x_Project.h"
//#include "EMIF_Module.h"
//#include "fpu_vector.h"  // FPU headerfile to access memcpy_fast_far()
//#include "Memory_Function.h"
//#include "EXTIO_Module.h"
//#include "SPI_Module.h"
//#include "CLA_Module.h"
//#include "CPU_Timer_Module.h"
//#include "EPWM_Module.h"
//#include "GPIO_Module.h"
//#include "DSP_ADC_16bit_Module.h"
//#include "EEPROM_Module.h"
//#include "SCI_Module.h"
//#include "stdio.h"
//#include "string.h"
//#include "Main_State_Module.h"
//#include "Output_Control_Module.h"
//#include "Fan_Control_Module.h"
//#include "Protect_Module.h"
//#include "Event_Module.h"
//#include "Wave_Generate_Module.h"
//#include "math.h"
//#include "Meas_Module.h"
//#include "Calibration_Module.h"
//#include "Output_ACDC_INT_Module.h"
//#include "Digital_Filter_Module.h"
//#include "PID_Control_Module.h"
//#include "Output_AC_VCA_Module.h"
//#include "Output_ACDC_SYNC_Module.h"
//#include "Output_ACDC_EXT_Module.h"
//#include "Output_ACDC_ADD_Module.h"
//#include "Output_SEQ_Module.h"
//#include "MS_Control_Module.h"
//#include "MS_Output_ACDC_INT_Module.h"
//#include "eCAP_Module.h"
//#include "Phase_Sync_Module.h"
//#include "main.h"
//#include "DAC_Module.h"
//
//#include "bf_common.h"
//#include "bf_interrupt.h"
//#include "bf_cputimer.h"
//#include "bf_gpio.h"
//#include "bf_spi.h"
//#include "bf_mcbsp.h"
//
//#include "timetask.h"
//#include "spi_master.h"
//#include "cTimeMeas.h"

/////////////////////////////////////////////////////////////////////
//
// 定義 for 機種規格
//
#define ASR_5075_MAX_AC_V       350.0
#define ASR_5075_MAX_DC_V       500.0
#define ASR_5075_MIN_DC_V       -500.0
#define ASR_5075_MAX_I          7.5
#define ASR_5075_MAX_I_PEAK     30.0
#define ASR_5075_MAX_P          750.0

#define ASR_5150_LR_MAX_AC_V    350.0
#define ASR_5150_LR_MAX_DC_V    500.0
#define ASR_5150_LR_MIN_DC_V    -500.0
#define ASR_5150_LR_MAX_I       15.0
#define ASR_5150_LR_MAX_I_PEAK  60.0
#define ASR_5150_LR_MAX_P       1500.0

#define ASR_5150_HR_MAX_AC_V    700.0
#define ASR_5150_HR_MAX_DC_V    1000.0
#define ASR_5150_HR_MIN_DC_V    -1000.0
#define ASR_5150_HR_MAX_I       7.5
#define ASR_5150_HR_MAX_I_PEAK  30.0
#define ASR_5150_HR_MAX_P       1500.0


/////////////////////////////////////////////////////////////////////
//
// 定義 for range
//
#define LOW_RANGE   0
#define HIGH_RANGE  1

#define FIX_LOW_RANGE   0
#define FIX_HIGH_RANGE  1
#define AUTO_RANGE      2



/////////////////////////////////////////////////////////////////////
//
// 定義 for Sync_freq_Source
//
#define SYNC_LINE   0
#define SYNC_EXT    1


/////////////////////////////////////////////////////////////////////
//
// 定義 for wave data
//
#define WAVE_ARB1   1
#define WAVE_ARB2   2
#define WAVE_ARB3   3
#define WAVE_ARB4   4
#define WAVE_ARB5   5
#define WAVE_ARB6   6
#define WAVE_ARB7   7
#define WAVE_ARB8   8
#define WAVE_ARB9   9
#define WAVE_ARB10  10
#define WAVE_ARB11  11
#define WAVE_ARB12  12
#define WAVE_ARB13  13
#define WAVE_ARB14  14
#define WAVE_ARB15  15
#define WAVE_ARB16  16
#define WAVE_SIN    17 //對應WAVE DATA 0
#define WAVE_SQU    18 //對應WAVE DATA 1
#define WAVE_TRI    19 //對應WAVE DATA 2


/////////////////////////////////////////////////////////////////////
//
// 定義 for 輸出Mode
//
#define DC_INT_MODE                         1
#define AC_INT_MODE                         2
#define ACDC_INT_MODE                       3
#define AC_EXT_MODE                         6
#define ACDC_EXT_MODE                       7
#define AC_ADD_MODE                         14
#define ACDC_ADD_MODE                       15
#define AC_SYNC_MODE                        18
#define ACDC_SYNC_MODE                      19
#define SEQ_MODE                            32
#define AC_VCA_MODE                         66
#define MASTER_DC_INT_MODE                  129
#define MASTER_AC_INT_MODE                  130
#define MASTER_ACDC_INT_MODE                131
#define SLAVE_DC_INT_MODE                   257
#define SLAVE_AC_INT_MODE                   258
#define SLAVE_ACDC_INT_MODE                 259
#define MASTER_DC_INT_SDRAME_SYNC_MODE      641
#define MASTER_AC_INT_SDRAME_SYNC_MODE      642
#define MASTER_ACDC_INT_SDRAME_SYNC_MODE    643
#define SLAVE_DC_INT_SDRAME_SYNC_MODE       769
#define SLAVE_AC_INT_SDRAME_SYNC_MODE       770
#define SLAVE_ACDC_INT_SDRAME_SYNC_MODE     771




/////////////////////////////////////////////////////////////////////
//
// 定義 for Slew Rate
//
#define SOFT_RAMP               0
#define FIX_STEP                1
#define SLEW_RATE_SET_USER_MAX  10000
#define SOFT_RAMP_COEFF         3.0

/////////////////////////////////////////////////////////////////////
//
// 定義 for 上下限值
//
#define I_PEAK_HOLD_START_TIME_MAX  60000
#define I_PEAK_HOLD_START_TIME_MIN  1
#define FREQ_SET_USER_MAX           200000
#define FREQ_SET_USER_MIN           100
//#define START_PHASE_SET_USER_MAX    3599
//#define START_PHASE_SET_USER_MIN    0
//#define STOP_PHASE_SET_USER_MAX     3599
//#define STOP_PHASE_SET_USER_MIN     0
#define PHASE_SET_USER_MAX          3599
#define PHASE_SET_USER_MIN          0


/////////////////////////////////////////////////////////////////////
//
// 定義 for WAVE及VREF DAC的零點
//
#define WAVE_DAC_ZERO               32767
//0109
//#define WAVE_DAC_ZERO               0
#define VREF_DAC_ZERO               697
#define DAC_MAX_VALUE               65535.0

/////////////////////////////////////////////////////////////////////
//
// 定義 for IREF及DAC比例
//
#define IPEAK_to_DAC_RATIO ( 4095.0 / 273000.0 )


/////////////////////////////////////////////////////////////////////
//
// 定義 for DAC及ADC的數位值
//
#define DAC7612_MAX_VALUE 4095.0
#define AD5543_MAX_VALUE 65535.0
#define AD5543_HALF_VALUE 32767.0


/////////////////////////////////////////////////////////////////////
//
// 定義 for SEQ的狀態 (follow ASR-2000)
//
#define _Sequence_out_off       0
#define _Sequence_out_on        1
#define _Sequence_run           2
#define _Sequence_hold          3

/////////////////////////////////////////////////////////////////////
//
// 定義 for SEQ輸出Mode
//
#define SEQ_AC_MODE     0
#define SEQ_DC_MODE     1
#define SEQ_ACDC_MODE   2



/*----------------------------------------------*/
//
// 開機各狀態檢查等待時間
//

#define T_POWER_UP_DELAY_START   3000 //(ms)

#define T_POWER_UP_FAN_CHECK     1000 //(ms)
#define T_POWER_UP_AC_ON         1000 //(ms)
#define T_POWER_UP_AC_ON_2       1000 //(ms)
#define T_POWER_UP_CHECK_PFC     1000 //(ms)
#define POWER_UP_PFC_CHECK2_TIME    1000 //(ms)
#define POWER_UP_DCAC_CHECK_TIME    1000 //(ms)
#define POWER_UP_DCDC_CHECK_TIME    1000 //(ms)


#define PFC_DCDC_OFF_DELAY_TIME     100 //(ms)
#define ERROR_RELAY_OFF_DELAY_TIME  20 //(ms)



/*----------------------------------------------*/
//
// 輸出ON及OFF Delay時間 for Relay
//
#define OUTPUT_ON_DELAY_TIME    20 //(ms)
#define OUTPUT_OFF_DELAY_TIME    20 //(ms)
#define OUTPUT_OFF_DELAY_TIME0    20 //(ms)
#define OUTPUT_OFF_DELAY_TIME1    20 //(ms)
#define OUTPUT_OFF_DELAY_TIME2    20 //(ms)



/////////////////////////////////////////////////////////////////////
//
// 定義 for DC的c1_coeff
//

//#define DC_c1_coeff 0.683212757 //60Hz
#define DC_c1_coeff 11.3664 //1000Hz
//#define DC_inv_c1_coeff 0.087979 //1000Hz

#define DC_c1_final 36036.036 //1000Hz


/////////////////////////////////////////////////////////////////////
//
// 定義 for DDS

//修改DDS頻率
#define DDS_Freq       120.0f

#define DDS_MutiFreq   1000.0f
#define DDS_HZ         (DDS_Freq * DDS_MutiFreq)
#define TWO_POW_32     0xFFFFFFFF


#define DC_FREQ_STEP  TWO_POW_32 * DDS_MutiFreq / DDS_HZ

#define DEG_STEP (TWO_POW_32 / DDS_Freq)

#define _1HZ (TWO_POW_32 / DDS_HZ)


////150khz
//#define DC_FREQ_STEP 28633115
//// = 2^32 x 1000Hz / 150000Hz
//
//#define DEG_STEP 28633115.30
//// = 2^32 /150 degree
//
//#define _1HZ 28633.1153066
//// = 2^32 * 2.775e-6 = 2^32 / 150000


//#define DC_FREQ_STEP 11930465
//// = 2^32 x 1000Hz / 360000Hz

//#define DEG_STEP 11930465.0
// = 2^32 /360 degree

//#define _1HZ 11918.534246
// = 2^32 * 2.775e-6 = 2^32 / 360000


//#define DC_FREQ_STEP 35791394
//// = 2^32 x 1000Hz / 120000Hz
//
//#define DEG_STEP 35791394.13
//// = 2^32 /120 degree
//
//#define _1HZ 35791.3941333
//// = 2^32 * 2.775e-6 = 2^32 / 120000

/*----------------------------------------------*/
//
// 進入CC/CP包含磁滯的倍率
//
#define POWER_LIMIT_RATIO           1.01
#define IRMS_LIMIT_RATIO            1.06//1.016



/*----------------------------------------------*/

#define TTL_LEVEL_L                 0.8
#define TTL_LEVEL_H                 2.0
//#define TTL_LEVEL_L                 0.0
//#define TTL_LEVEL_H                 0.0


/*----------------------------------------------*/
//(Hz)
#define FREQ_COMPENSATION_LEVEL     100.0


/*----------------------------------------------*/
//
// MS Master/Slave 同步控制指令
//
/*
#define ON_Sync_Trig        1
#define Change_Sync_Trig_0  2
#define Change_Sync_Trig_1  3
#define OFF_Sync_Trig_0     4
#define OFF_Sync_Trig_1     5
*/

/*----------------------------------------------*/
//
// MS Output Phase 定義
//
#define _3P4W_3UNITS   0
#define _1P2W_2UNITS   1
#define _1P3W_2UNITS   2
#define _1P2W_3UNITS   3



/*----------------------------------------------*/
//
// POWER_FUNCTIONS_ERROR_CODE List
//
#define NO_ERROR_CODE                           0
#define CHECK_MACHINE_TYPE_NG                   1
#define CHECK_MS_RS485_CODE_AB_NG               2
#define WAIT_MS_RS485_CODE_CD_OVERTIME          3
#define WAIT_MS_RS485_CODE_77_OVERTIME          4
#define WAIT_MS_RS485_CODE_00_OVERTIME          5
#define WAIT_MS_RS485_CODE_01_OVERTIME          6
#define WAIT_MS_RS485_CODE_02_OVERTIME          7
#define WAIT_MS_RS485_CODE_80_OVERTIME          8
#define WAIT_MS_RS485_CODE_81_OVERTIME          9
#define WAIT_MS_RS485_CODE_82_OVERTIME          10
#define WAIT_MS_RS485_CODE_83_OVERTIME          11
#define WAIT_MS_RS485_CODE_84_OVERTIME          12
#define WAIT_MS_RS485_CODE_AB_OVERTIME          13
#define WAIT_MS_RS485_CODE_BA_OVERTIME          14
#define SLAVE_RS485_RX_OVERTIME                 15
#define CHECK_MS_RS485_CODE_CD_NG               16
#define CHECK_MS_RS485_CODE_77_NG               17
#define CHECK_MS_RS485_CODE_00_NG               18
#define CHECK_MS_RS485_CODE_01_NG               19
#define CHECK_MS_RS485_CODE_02_NG               20
#define CHECK_MS_RS485_CODE_80_NG               21
#define CHECK_MS_RS485_CODE_81_NG               22
#define CHECK_MS_RS485_CODE_82_NG               23
#define CHECK_MS_RS485_CODE_83_NG               24
#define CHECK_MS_RS485_CODE_84_NG               25
#define CHECK_POWER_STATUS_NG                   26
#define WAIT_SLAVE_WAIT_ON_SYNC_OVERTIME        27
#define CHECK_FW_VER_NG                         28
#define MS_SYNC_SDRAM_CHECKSUM_NG               29
#define WAIT_MS_SPI_CODE_6AB7_OVERTIME          30
#define LOAD_WAVE_SIN_TO_LOCAL_CHECKSUM_NG      31
#define LOAD_WAVE_SQU_TO_LOCAL_CHECKSUM_NG      32
#define LOAD_WAVE_TRI_TO_LOCAL_CHECKSUM_NG      33
#define LOAD_WAVE_ARB1_TO_LOCAL_CHECKSUM_NG     34
#define LOAD_WAVE_ARB2_TO_LOCAL_CHECKSUM_NG     35
#define LOAD_WAVE_TO_LOCAL_CHECKSUM_NG          36
#define SDRAM_SELF_TEST_V2_NG                   37
#define CALI_CS_RS_OVERRANGE                    38
#define EEPROM_READ_CHECKSUM_NG                 39
#define EEPROM_WRITE_NG                         40
#define WRITE_CALI_VARIABLE_CHECKSUM_NG         41
#define WAIT_MS_SPI_CHECKSUM_OVERTIME           42
#define CALI_ADC_to_EXTIN_L1_offset_OVERRANGE   43
#define SET_CALI_STEP_0_ERROR                   44
#define CALI_ADC_to_V_FB_OVERRANGE              45
#define CALI_VSET_to_DAC_OVERRANGE              46
#define CALI_ADC_to_I_FB_OVERRANGE              47
#define CHECK_MS_ADDRESS_NG                     48
#define WAIT_MS_SPI_ON_Sync_Trig_OVERTIME       49
#define WAIT_MS_SPI_Change_Sync_Trig_0_OVERTIME 50
#define WAIT_MS_SPI_Change_Sync_Trig_1_OVERTIME 51
#define WAIT_MS_SPI_OFF_Sync_Trig_0_OVERTIME    52
#define WAIT_MS_SPI_OFF_Sync_Trig_1_OVERTIME    53
#define CHECK_MS_RS485_CHECKSUM_NG              54
#define WAIT_Slave_Wait_Change_Sync_OVERTIME    55
#define WAIT_Slave_Wait_OFF_Sync_OVERTIME       56


typedef short           int16;
typedef long            int32;
typedef unsigned char   Uint8;
typedef unsigned short  Uint16;
typedef unsigned long   Uint32;
typedef float           float32;
typedef long double     float64;





/////////////////////////////////////////////////////////////////////

//----------------------------------------------------
typedef union
{
    float32 all;
    struct
    {
        Uint16 LSB:16;
        Uint16 MSB:16;
    };
}_float32_MSB_LSB;

//----------------------------------------------------
typedef union
{
    Uint32 all;
    struct
    {
        Uint16 LSB:16;
        Uint16 MSB:16;
    };
}_Uint32_MSB_LSB;

//----------------------------------------------------
typedef union
{
    int32 all;
    struct
    {
        Uint16 LSB:16;
        Uint16 MSB:16;
    };
}_int32_MSB_LSB;


//----------------------------------------------------
//
//
//
typedef union
{
    Uint32 all;

    struct
    {
        Uint32 dec2:12;
        Uint32 dec1:8;
        Uint32 addr:12;
    };

    //
    // IV 取樣1k
    //
    struct
    {
        Uint32 dec3:22;
        Uint32 addr_1k:10;
    };

}DDSDATA;




//----------------------------------------------------
//機器規格參數
typedef struct MACHINE_SERIES_IMFORMATION_VARIABLES_REG{

    Uint16 Machine_series; //0x953
    //ASR5000系列： 0為 ASR5075、1為 ASR5150、2為ASR5151

    float AC_Rated_Vout;

    float DC_Maximum_Vout;
    float DC_Minimum_Vout;

    float Maximum_Iout;
    float Maximum_Iout_peak;
    float Rated_Power;


}MACHINE_SERIES_IMFORMATION_VARIABLES_REG;


//----------------------------------------------------
//
// 0x0421 Startup State
//
// 0 => Checking
// 1 => OK
// 2 => NG

enum {
    _CHECKING = 0x0,
    _OK = 0x1,
    _NG = 0x2,
};

struct  STARTUP_STATE_FLAG_BITS {   // bit
    Uint16 AC_Input_Check:2;    // 0-1
    Uint16 Fan_Check:2;         // 2-3
    Uint16 PFC_Check:2;         // 4-5
    Uint16 DCDC_Check:2;        // 6-7
    Uint16 DCAC_Check:2;        // 8-9
    Uint16 SDRAM_Check:2;       // 10-11
    Uint16 EEPROM_Check:2;      // 12-13

};
union STARTUP_STATE_FLAG_REG {
   Uint16                                   all;
   struct STARTUP_STATE_FLAG_BITS   bit;
};



//----------------------------------------------------
//
// 0x092D CPU State
//
struct  CPU_STATE_FLAG_BITS {   // bit
    Uint16 Software_Busy:1;    // 0
    Uint16 Software_Update:1;  // 1

};
union CPU_STATE_FLAG_REG {
   Uint16                       all;
   struct CPU_STATE_FLAG_BITS   bit;
};




//----------------------------------------------------
//輸入IO
struct INPUT_FLAG_BITS {    // bits
    Uint16 FANFAIL2:1;      // 0 for 半寬機種, 5075
    Uint16 FANFAIL1:1;      // 1 for 半寬機種, 5075
    Uint16 PFC_UVPn:1;       // 2 for 半寬機種, 5075
    Uint16 PFC_OVPn:1;       // 3 for 半寬機種, 5075
    Uint16 PFC_OTPn:1;       // 4
    Uint16 ACFAILn:1;        // 5

    Uint16 FANFAIL:1;       // 6 for 全寬機種, 5150, 5151
    Uint16 PFC_UVP1n:1;      // 7 for 全寬機種, 5150, 5151
    Uint16 PFC_OVP1n:1;      // 8 for 全寬機種, 5150, 5151
    Uint16 PFC_UVP2n:1;      // 9 for 全寬機種, 5150, 5151
    Uint16 PFC_OVP2n:1;      // 10 for 全寬機種, 5150, 5151

    Uint16 DC_OVP:1;        // 11

    Uint16 ACFAIL_shadow1:1;// 12
    Uint16 ACFAIL_shadow2:1;// 13
    Uint16 ACFAIL_shadow3:1;// 14

};

union INPUT_FLAG_REG {
   Uint16                 all;
   struct INPUT_FLAG_BITS bit;
};


//----------------------------------------------------
//輸出IO
struct OUTPUT_FLAG_BITS {   // bits
    Uint16 OUT_ON:1;        // 0
    Uint16 REMOTE:1;        // 1
    Uint16 IM_HI_LO:1;      // 2
    Uint16 VM_HI_LO:1;      // 3
    Uint16 ADCTRL5:1;       // 4
    Uint16 PFC_ON2:1;       // 5 for 全寬機種, 5150, 5151
    Uint16 DCDC_ON2:1;      // 6 for 全寬機種, 5150, 5151
    Uint16 OUT_HI_LO:1;     // 7 for 全寬機種, 5150, 5151

    Uint16 PFC_ON:1;        // 8
    Uint16 DCDC_ON:1;       // 9
    Uint16 DCAC_ENA:1;      // 10
    Uint16 AC_OCPA:1;       // 11
    Uint16 AC_ON:1;         // 12 for 全寬機種, 5150, 5151
    Uint16 RSV_06:1;        // 13
    Uint16 RSV_05:1;        // 14
    Uint16 RSV_04:1;        // 15

};

union OUTPUT_FLAG_REG {
   Uint16                  all;
   struct OUTPUT_FLAG_BITS bit;
};


//----------------------------------------------------
//
//EXT輸入IO
//0x702 External Input
//
struct EXT_INPUT_FLAG_BITS {          // bits
    Uint16 UNDEFINED_IN0:1;     // 0
    Uint16 OUTPUT_OFF:1;        // 1
    Uint16 OUTPUT_ON:1;         // 2
    Uint16 SEQ_START:1;         // 3
    Uint16 SEQ_STOP:1;          // 4
    Uint16 SEQ_HOLD:1;          // 5
    Uint16 BRANCH1:1;           // 6
    Uint16 BRANCH2:1;           // 7

};

union EXT_INPUT_FLAG_REG {
   Uint16                     all;
   struct EXT_INPUT_FLAG_BITS bit;
};





//----------------------------------------------------
//
//EXT輸出IO
//0x703 External Output
//
struct EXT_OUTPUT_FLAG_BITS {        // bits
    Uint16 POWER_ON:1;              // 0
    Uint16 OUTPUT_ON:1;             // 1
    Uint16 I_LIMITER:1;             // 2
    Uint16 SOFTWARE_BUSY:1;         // 3
    Uint16 SEQ_SYNC0:1;             // 4
    Uint16 SEQ_SYNC1:1;             // 5
    Uint16 ZERO_CROSS_TRIG_OUT:1;   // 6
    Uint16 UNDEFINED_OUT1:1;        // 7
};

union EXT_OUTPUT_FLAG_REG {
   Uint16                      all;
   struct EXT_OUTPUT_FLAG_BITS bit;
};


//----------------------------------------------------
//
//Power控制
//0x900 Output ON/OFF
//
struct POWER_CONTROL_BITS {         // bits
    Uint16 OUTPUT_ON:1;             // 0
    //Uint16 TRIG_OUTPUT_ON:1;        // 1 //程式內部自動輸出使用

    Uint16 OUTPUT_ON_user:1;        // 2 //使用者設定ON/OFF
    Uint16 OUTPUT_ON_user_shadow:1; // 3
};

union POWER_CONTROL_REG {
   Uint16                    all;
   struct POWER_CONTROL_BITS bit;
};


//----------------------------------------------------
// Power狀態
// 0x401 Machine Status
//
struct POWER_STATUS_BITS {          // bits
    Uint16 Output_ON:1;             // 0
    Uint16 V_Range:1;               // 1
    Uint16 I_Range:1;               // 2
    Uint16 Rsv_01:1;                // 3
    Uint16 I_peak_limit:1;          // 4
    Uint16 Remote_Sense_Limit:1;    // 5
    Uint16 Output_ONOFF_Busy:1;     // 6
    Uint16 PWM_Enable:1;            // 7
    Uint16 Control_Mode:2;          // 8-9
                                    //CV (00) / CC (01) / CP (10)
    Uint16 Range_Change_Busy:1;     // 10
    Uint16 VoltFreq_Change_Busy:1;  // 11
    Uint16 MS_Master:1;               // 12
};

union POWER_STATUS_REG {
   Uint16                    all;
   struct POWER_STATUS_BITS bit;
};


//----------------------------------------------------
// 輸出模式
// 0x903
// 1 為DC-INT、2 為AC-INT、3 為AC+DC-INT
// 6 為AC-Ext、7 為AC+DC-Ext,
// 14 為AC-ADD、15 為AC+DC-ADD
// 18 為AC-SYNC、19 為AC+DC-SYNC
// 32 為Sequence、66 為AC-VCA模式
// 129 MS Master DC-INT
// 130 MS Master AC-INT
// 131 MS Master AC+DC-INT
// 257 MS Slave DC-INT
// 258 MS Slave AC-INT
// 259 MS Slave AC+DC-INT
//
struct OUTPUT_MODE_FLAG_BITS {    // bits
    Uint16 DC:1;            // 0
    Uint16 AC:1;            // 1
    Uint16 Ext:1;           // 2
    Uint16 ADD:1;           // 3
    Uint16 SYNC:1;          // 4
    Uint16 SEQ:1;           // 5
    Uint16 VCA:1;           // 6
    Uint16 MS_Master:1;     // 7
    Uint16 MS_Slave:1;      // 8
    Uint16 MS_SDRAM_SYNC:1; // 9
};

union OUTPUT_MODE_FLAG_REG {
   Uint16                 all;
   struct OUTPUT_MODE_FLAG_BITS bit;
};




//----------------------------------------------------
//
// Zero Cross Option
//
struct ZERO_CROSS_OPTION_BITS {         // bits
    Uint16 Rsv_01:1;                    // 0
    Uint16 Rsv_02:1;                    // 1
    Uint16 H_L_Level:1;                 // 2

};

union ZERO_CROSS_OPTION_REG {
   Uint16                           all;
   struct ZERO_CROSS_OPTION_BITS    bit;
};




//----------------------------------------------------
//AD類比輸入
typedef struct  INPUT_VARIABLES_REG {
    float V_FB;
    float I_FB;
    float Temp_Sensor;
}INPUT_VARIABLES_REG;


//----------------------------------------------------
//
//EXT AD類比輸入
//
//
typedef struct  EXT_INPUT_VARIABLES_REG {

    Uint16 EXT_AD_IN;    //0x704 External AD
    Uint16 EXT_AD_IN_TTL_Level_L;
    Uint16 EXT_AD_IN_TTL_Level_H;

    _float32_MSB_LSB EXT_Freq;     //0x708 External Frequency
    float EXT_Freq_shadow;

    float EXT_ACV_Set;
    float EXT_ACV_Set_shadow;

    Uint16 EXT_AD_DC_Filter_DC_Level;
    Uint16 EXT_AD_DC_Filter_LPF_Out;

}EXT_INPUT_VARIABLES_REG;



//----------------------------------------------------
//DA類比輸出
typedef struct  OUTPUT_VARIABLES_REG {

    Uint16 VREF_set;
    Uint16 VREF_set_shadow;

    int32 WAVE_set;
    int32 WAVE_offset;
    int32 WAVE_offset_shadow;
    float WAVE_scale;
    float WAVE_scale_shadow;
    int32 WAVE_goal;
    int32 WAVE_goal_shadow;

    float WAVE_set_diff_level;

    float ext_WAVE_scale;
    float ext_WAVE_scale_shadow;

    float p_IREF_set;
    float n_IREF_set;
}OUTPUT_VARIABLES_REG;


//----------------------------------------------------
//設定命令
typedef struct POWER_OUTPUT_SETTING_VARIABLES_REG{
    _int32_MSB_LSB ACV_set_user;             //0x93B
    Uint16 ACV_set_user_LSB_buf; //for cmd
    int32 ACV_set_user_max;
    int32 ACV_set_user_min;
    int32 ACV_set_user_shadow;

    _int32_MSB_LSB DCV_set_user;             //0x939
    Uint16 DCV_set_user_LSB_buf; //for cmd
    int32 DCV_set_user_max;
    int32 DCV_set_user_min;
    int32 DCV_set_user_shadow;

    Uint16 V_SlewRate_set_user; //0x927
    Uint16 V_SlewRate_set_user_shadow;
    float V_SoftRamp_Step_set;
    float Output_OFF_Vref_SlewRate_step;
    float Soft_Ramp_Coeff;

    _Uint32_MSB_LSB FREQ_set_user;           //0x910
    Uint16 FREQ_set_user_LSB_buf; //for cmd
    Uint32 FREQ_set_user_shadow;

    Uint16 Start_Phase_set_user;    //0x920
    Uint16 Start_Phase_set_user_shadow;
    Uint32 Start_phase_count;
    Uint32 Start_phase_count_shadow;

    Uint16 Stop_Phase_set_user;     //0x922
    Uint16 Stop_Phase_set_user_shadow;
    Uint32 Stop_phase_count;

    Uint16 SYNC_Phase_Delay_set_user;     //0x942
    Uint16 SYNC_Phase_Delay_set_user_shadow;
    Uint32 SYNC_Phase_Delay_Count;

    Uint32 SYNC_Zero_phase_count;


    Uint16 FanSpeed_set_user;

    _int32_MSB_LSB Ext_GAIN_set_user;        //0x937
    int32 Ext_GAIN_set_user_shadow;
    Uint16 Ext_GAIN_set_user_LSB_buf; //for cmd
    int32 Ext_GAIN_set_user_max;
    int32 Ext_GAIN_set_user_min;

    Uint16 Waveform_set_user;         //0x902
    Uint16 Waveform_set_user_shadow;

    Uint16 I_peak_hold_start_time;  //0x90A
    Uint16 V_peak_hold_start_time;  //0x913

    _int32_MSB_LSB VCA_GAIN_set_user;        //0x93D
    int32 VCA_GAIN_set_user_shadow;
    Uint16 VCA_GAIN_set_user_LSB_buf; //for cmd
    int32 VCA_GAIN_set_user_max;
    int32 VCA_GAIN_set_user_min;

    _Uint32_MSB_LSB V_Rise_Time_set_user;
    Uint16 V_Rise_Time_set_user_LSB_buf; //for cmd


    _float32_MSB_LSB Wave_CF_set_user[20]; //9E0
    //宣告20 => 0~19
    //實際使用1~19

    float Wave_CF_set[20];

    Uint16 Wave_data_page; //0x707, 0x958

    Uint16 Zero_Cross_Width_Set; //0x944 (參考ASR-6000)
    // 單位 100us

    Uint16 SQU_Duty_set_user;

    union ZERO_CROSS_OPTION_REG Zero_Cross_Option;

    Uint16 Output_Range_Option_set_user;
    // 0 -> Fix Low
    // 1 -> Fix High
    // 2 -> Auto ( 保留 )
    Uint16 Output_Range_Option;


    Uint16 Output_Range;
    // 0 -> Low ( 5150 )
    // 1 -> High ( 5151 )
    Uint16 Output_Range_shadow;


}POWER_OUTPUT_SETTING_VARIABLES_REG;


//----------------------------------------------------
//量測顯示
typedef struct POWER_DISPLAY_VARIABLES_REG{
    _float32_MSB_LSB FREQ_display;         //0x423

    _float32_MSB_LSB V_rms_display;        //0x403
    float            V_rms_display_shadow;

    _float32_MSB_LSB AC_V_rms_display;
    float            AC_V_rms_display_shadow;

    _float32_MSB_LSB V_avg_display;        //0x411
    float            V_avg_display_shadow;

    _float32_MSB_LSB p_V_peak_display;     //0x40D
    float            p_V_peak_display_shadow;

    _float32_MSB_LSB n_V_peak_display;     //0x40F
    float            n_V_peak_display_shadow;


    _float32_MSB_LSB I_rms_display;        //0x405
    float            I_rms_display_shadow;

    _float32_MSB_LSB I_avg_display;        //0x413
    float            I_avg_display_shadow;

    _float32_MSB_LSB p_I_peak_display;     //0x407
    float            p_I_peak_display_shadow;

    _float32_MSB_LSB n_I_peak_display;     //0x40B
    float            n_I_peak_display_shadow;

    _float32_MSB_LSB P_display;            //0x409 W
    _float32_MSB_LSB P_display_shadow;

    _float32_MSB_LSB S_display;            //0x415 VA
    float            S_display_shadow;

    _float32_MSB_LSB Q_display;            //0x417 VAR
    float            Q_display_shadow;

    _float32_MSB_LSB Temp_display;         //0x42B 溫度

    int16 PF_display;                      //0x419 注意是int16, 0.001
    int16 CF_display;                      //0x41B 注意是int16

    _float32_MSB_LSB I_peak_hold_display;  //0x41D

    _float32_MSB_LSB V_peak_hold_display;  //0x42D

    Uint16 VI_Raw_Data_Counter;             //0x701

    _float32_MSB_LSB  Short_OCP_V_FB;           //0x417
    Uint16            Short_OCP_c2;             //0x419
    Uint16            Short_OCP_VREF_set;       //0x41B
    Uint16            Short_OCP_WAVE_set;

}POWER_DISPLAY_VARIABLES_REG;


//----------------------------------------------------
//保護設定
typedef struct  POWER_PROTECT_SETTING_VARIABLES_REG {
    _int32_MSB_LSB p_I_peak_limit_level;         //0x933
    int32 p_I_peak_limit_level_shadow;
    Uint16 p_I_peak_limit_level_LSB_buf; //for cmd
    Uint32 p_I_peak_limit_delay_time;
    int32 p_I_peak_limit_level_max;
    Uint32 p_I_peak_ocp_start_time;


    _int32_MSB_LSB n_I_peak_limit_level;         //0x935
    int32 n_I_peak_limit_level_shadow;
    Uint16 n_I_peak_limit_level_LSB_buf; //for cmd
    Uint32 n_I_peak_limit_delay_time;
    int32 n_I_peak_limit_level_max;         //0x935
    Uint32 n_I_peak_ocp_start_time;

    //float p_I_short_level;
    //Uint32 p_I_short_delay_time;
    //Uint32 p_I_short_ocp_start_time;

    //float n_I_short_level;
    //Uint32 n_I_short_delay_time;
    //Uint32 n_I_short_ocp_start_time;

    _int32_MSB_LSB I_rms_limit_level;            //0x931
    int32  I_rms_limit_level_shadow;
    int32  I_rms_limit_level_shadow2;
    Uint16 I_rms_limit_level_LSB_buf; //for cmd
    Uint32 I_rms_limit_delay_time;
    int32 I_rms_limit_level_max;
    Uint32 I_rms_ocp_start_time;

    float I_limit_level;
    Uint32 I_limit_delay_time;

    float _4CF_OCP_level;

    //float p_V_peak_limit_level;
    //Uint32 p_V_peak_limit_delay_time;

    //float n_V_peak_limit_level;
    //Uint32 n_V_peak_limit_delay_time;

    float V_rms_limit_level;
    Uint32 V_rms_limit_delay_time;
    Uint32 V_rms_ovp_start_time;


    float OPP_level;
    Uint32 OPP_delay_time;
    Uint32 opp_start_time;

    float Power_limit_level;
    float Negative_OPP_Level;
    float Negative_OPP_Level_shadow;

    float remote_OVP_level;
    Uint32 remote_OVP_delay_time;
    Uint32 remote_OVP_start_time;

    Uint32 FANFAIL1_delay_time;
    Uint32 FANFAIL2_delay_time;
    Uint32 PFC_UVP_delay_time;
    Uint32 PFC_OVP_delay_time;
    Uint32 PFC_OTP_delay_time;
    Uint32 ACFAIL_delay_time;

    Uint32 FANFAIL_delay_time; //for 全寬機種
    Uint32 PFC_UVP1_delay_time; //for 全寬機種
    Uint32 PFC_OVP1_delay_time; //for 全寬機種
    Uint32 PFC_UVP2_delay_time; //for 全寬機種
    Uint32 PFC_OVP2_delay_time; //for 全寬機種

    Uint32 DC_OVP_delay_time;


    float DCAC_OTP_level;
    Uint32 DCAC_OTP_delay_time;

    Uint32 u32Fan1FailTstamp;
    Uint32 u32Fan2FailTstamp;
    Uint32 u32PfcUvpTstamp;
    Uint32 u32PfcOvpTstamp;
    Uint32 u32POtpTstamp;
    Uint32 u32AcFailTstamp;
    Uint32 DCAC_OTP_start_time;

    Uint32 u32FanFailTstamp; //for 全寬機種
    Uint32 u32Pfc1UvpTstamp; //for 全寬機種
    Uint32 u32Pfc1OvpTstamp; //for 全寬機種
    Uint32 u32Pfc2UvpTstamp; //for 全寬機種
    Uint32 u32Pfc2OvpTstamp; //for 全寬機種

    Uint32 DC_OVP_start_time;

}POWER_PROTECT_SETTING_VARIABLES_REG;


//----------------------------------------------------
//
//異常旗標
//0x429 C2000 Alarm Status
//
struct  POWER_FUNCTIONS_ERROR_FLAG_BITS {   // bit
    Uint16 P_I_PEAK_OCP_SW:1;               // 0
    Uint16 N_I_PEAK_OCP_SW:1;               // 1
    Uint16 PFC_OTP_HW:1;                    // 2
    Uint16 I_RMS_OCP_SW:1;                  // 3
    Uint16 OUTPUT_ON_ACFAIL_HW:1;           // 4
    Uint16 RSV_05:1;                        // 5
    Uint16 OPP_SW:1;                        // 6
    Uint16 SHORT_SW:1;                      // 7
    Uint16 OVP_SW:1;                        // 8
    Uint16 CALIBRATION_ERROR_SW:1;          // 9
    Uint16 DCAC_OTP_SW:1;                   // 10
    Uint16 RSV_11:1;                        // 11
    Uint16 REMOTE_OVP_SW:1;                 // 12
    Uint16 RSV_13:1;                        // 13
    Uint16 OUTPUT_OFF_ACFAIL_HW:1;          // 14
    Uint16 SDRAM_ERROR_SW:1;                // 15


};
union POWER_FUNCTIONS_ERROR_FLAG_REG {
   Uint16                                   all;
   struct POWER_FUNCTIONS_ERROR_FLAG_BITS   bit;
};

typedef enum {
    SW_ERR1_P_I_PEAK_OCP       = (0x0001<<0),
    SW_ERR1_N_I_PEAK_OCP       = (0x0001<<1),
    HW_ERR1_PFC_OTP            = (0x0001<<2),
    SW_ERR1_I_RMS_OCP          = (0x0001<<3),
    HW_ERR1_OUTPUT_ON_ACFAIL   = (0x0001<<4),
    SW_ERR1_RSV_05             = (0x0001<<5),
    SW_ERR1_OPP                = (0x0001<<6),
    SW_ERR1_SHORT              = (0x0001<<7),
    SW_ERR1_OVP                = (0x0001<<8),
    SW_ERR1_CALIBRATION_ERROR  = (0x0001<<9),
    SW_ERR1_DCAC_OTP           = (0x0001<<10),
    SW_ERR1_RSV_11             = (0x0001<<11),
    SW_ERR1_REMOTE_OVP         = (0x0001<<12),
    SW_ERR1_RSV_13             = (0x0001<<13),
    HW_ERR1_OUTPUT_OFF_ACFAIL  = (0x0001<<14),
    SW_ERR1_SDRAM_ERROR        = (0x0001<<15),
    HW_CRITICAL_ERROR1 = (HW_ERR1_OUTPUT_ON_ACFAIL|HW_ERR1_OUTPUT_OFF_ACFAIL|HW_ERR1_PFC_OTP),
    SW_COMMON_ERROR1 = (SW_ERR1_P_I_PEAK_OCP|SW_ERR1_N_I_PEAK_OCP|SW_ERR1_I_RMS_OCP|SW_ERR1_OPP|SW_ERR1_SHORT|SW_ERR1_OVP| \
                        SW_ERR1_CALIBRATION_ERROR|SW_ERR1_DCAC_OTP|SW_ERR1_REMOTE_OVP|SW_ERR1_SDRAM_ERROR)
} ERROR1_MASK;



//----------------------------------------------------
//
//異常旗標2 for 全寬機種
//0x42A C2000 Alarm Status 2

enum {
    _NO_ERRORG = 0x0,
    _FAIL = 0x1,
    _NO_ERROR_N = 0x1,
    _FAIL_N = 0x0
};

struct  POWER_FUNCTIONS_ERROR_FLAG2_BITS {   // bit
    Uint16 FANFAIL1_HW:1;                    // 0
    Uint16 FANFAIL2_HW:1;                    // 1
    Uint16 PFC_UVP1_HW:1;                    // 2
    Uint16 PFC_UVP2_HW:1;                    // 3
    Uint16 PFC_OVP1_HW:1;                    // 4
    Uint16 PFC_OVP2_HW:1;                    // 5
    Uint16 DC_OVP_HW:1;                      // 6
    Uint16 MS_CONNECT_STATE_Error:1;         // 7
    Uint16 MS_CONNECT_OK_STATE_Error:1;      // 8
    Uint16 MS_SDRAM_SYNC_STATE_Error:1;      // 9
};

union POWER_FUNCTIONS_ERROR_FLAG2_REG {
   Uint16                                   all;
   struct POWER_FUNCTIONS_ERROR_FLAG2_BITS   bit;

   struct {
       Uint16 HwFanFail:2;
       Uint16 HwPfcUvp:2;
       Uint16 HwPfcOvp:2;
       Uint16 HwDcOvp:1;
       Uint16 HwMS:3;
   } type;
};

typedef enum {
    HW_ERR2_FANFAIL1 = (0x0001<<0),
    HW_ERR2_FANFAIL2 = (0x0001<<1),
    HW_ERR2_PFC_UVP1 = (0x0001<<2),
    HW_ERR2_PFC_UVP2 = (0x0001<<3),
    HW_ERR2_PFC_OVP1 = (0x0001<<4),
    HW_ERR2_PFC_OVP2 = (0x0001<<5),
    HW_ERR2_DC_OVP   = (0x0001<<6),
    HW_ERR2_MS_CONNECT_STATE_Error = (0x0001<<7),
    HW_ERR2_MS_CONNECT_OK_STATE_Error = (0x0001<<8),
    HW_ERR2_MS_SDRAM_SYNC_STATE_Error = (0x0001<<9),
    HW_CRITICAL_ERROR2 = (HW_ERR2_FANFAIL1|HW_ERR2_FANFAIL2| \
                          HW_ERR2_PFC_UVP1|HW_ERR2_PFC_UVP2| \
                          HW_ERR2_PFC_OVP1|HW_ERR2_PFC_OVP2| \
                          HW_ERR2_DC_OVP),
    HW_PARALLEL_ERROR2 = (HW_ERR2_MS_CONNECT_STATE_Error| \
                          HW_ERR2_MS_CONNECT_OK_STATE_Error| \
                          HW_ERR2_MS_SDRAM_SYNC_STATE_Error),

} ERROR2_MASK;

//----------------------------------------------------
//功能啟用開關
struct  POWER_FUNCTIONS_ENABLE_FLAG_BITS {  // bit
    Uint16 Protection_Reset:1;              //0 0x952
    Uint16 Protection_Enable:1;             //1
    Uint16 Calibration_Reset:1;             //2
    Uint16 Factory_Reset:1;                 //3
    Uint16 I_peak_hold_clear:1;             //4 0x909
    Uint16 I_rms_limit_Enable:1;            //5 0x90E
    Uint16 I_peak_limit_Enable:1;           //6 0x90F
    Uint16 Ext_IO_Control_Enable:1;         //7 0x918
    Uint16 Start_Phase_Enable:1;            //8 0x921
    Uint16 Stop_Phase_Enable:1;             //9 0x923

    Uint16 V_SlewRate_Mode:1;               //10 0x92A
    //Mode 0 (0) 為固定爬升時間模式
    //Mode 1 (1) 為固定爬升斜率模式
    //預設為 0

    Uint16 Sync_freq_Source:1;              //11 0x92B
    //設定SYNC頻率來源，Line (0) / External (1)

    Uint16 Remote_Sense_Enable:1;           //12 0x92C
    Uint16 ACV_Unit:1;                      //13 0x94A
    //RMS (0) / Vpp (1)

    Uint16 IMONI_LO:1;                      //14
    Uint16 VMONI_LO:1;                      //15

};
union POWER_FUNCTIONS_ENABLE_FLAG_REG {
   Uint16                                    all;
   struct POWER_FUNCTIONS_ENABLE_FLAG_BITS   bit;
};


//----------------------------------------------------
//功能啟用開關 2
struct  POWER_FUNCTIONS_ENABLE_FLAG2_BITS {  // bit

    Uint16 Freq_Compensation_Enable:1;      // 1 0x929
    Uint16 CV_Compensation_Enable:1;        // 2 0x926
    Uint16 CV_Compensation_Enable_shadow:1; // 3
    Uint16 V_peak_hold_clear:1;             // 4 0x912

    Uint16 Zero_Change_Phase_Enable:1;      // 5 0x919

    Uint16 WAVE_DAC_Slew_Rate_Enable:1;     // 6
    Uint16 VREF_DAC_Slew_Rate_Enable:1;     // 7

    Uint16 MS_Enable:1;                     // 8
    Uint16 MS_Balance_Compensate_Enable:1;  // 9
    Uint16 MS_Master:1;                     // 10

    Uint16 Display_Modify_Enable:1;         // 11
    Uint16 Output_Relay_Enable:1;           // 12
};
union POWER_FUNCTIONS_ENABLE_FLAG2_REG {
   Uint16                                    all;
   struct POWER_FUNCTIONS_ENABLE_FLAG2_BITS   bit;
};







//----------------------------------------------------
//V I raw op code
//0x801
struct  VI_RAW_OP_CODE_BITS {  // bit
    Uint16 Unused_0:1;                //00
    Uint16 Unused_1:1;                //01
    Uint16 Trig:1;                  //02
    Uint16 raw_data_4k_1k:1;          //03

};
union VI_RAW_OP_CODE_REG {
   Uint16                                    all;
   struct VI_RAW_OP_CODE_BITS   bit;
};


//RD Mode IO Control
// 0x880
struct RD_Mode_IO_Control_BITS {  // bit
    Uint16 OFF_PWM:1;                //00
    Uint16 Master:1;                //01


};
union RD_Mode_IO_Control_REG {
   Uint16                                    all;
   struct RD_Mode_IO_Control_BITS   bit;
};





//----------------------------------------------------
//
// 為節省CPU資源,預先計算的參數
//
typedef struct  PRE_CALC_VARIABLES_REG {

    float inv_Wave_TRI_CF_set_user;
    float inv_ADC_to_EXTIN_L1_Scale;
    float inv_Vmax;

}PRE_CALC_VARIABLES_REG;




//----------------------------------------------------
//
// 校正參數
//
typedef struct  CALIBRATION_VARIABLES_REG {
    Uint16            CheckSum;                     //0x430 0xA10

    Uint16            Data_Version;                 //0x431 0xA11

    // 此區SPI先傳送LSB再MSB

    _float32_MSB_LSB  ADC_to_V_FB_Scale_P;          //0x432
    _float32_MSB_LSB  ADC_to_V_FB_Scale_N;          //0x434
    _float32_MSB_LSB  ADC_to_V_FB_Offset;           //0x436

    _float32_MSB_LSB  ADC_to_I_FB_Scale_P;          //0x438
    _float32_MSB_LSB  ADC_to_I_FB_Scale_N;          //0x43A
    _float32_MSB_LSB  ADC_to_I_FB_Offset;           //0x43C
    _float32_MSB_LSB  Cs_mpy_360000;                //0x43E
    _float32_MSB_LSB  inv_Rs;                       //0x440

    _float32_MSB_LSB  VSET_to_DAC_Scale;            //0x442
    _float32_MSB_LSB  VSET_to_DAC_Offset;           //0x444

    _float32_MSB_LSB  Freq_Compensation_coeff_a;    //0x446
    _float32_MSB_LSB  Freq_Compensation_coeff_b;    //0x448
    _float32_MSB_LSB  Freq_Compensation_coeff_c;    //0x44A
    _float32_MSB_LSB  V_rms_Offset;                 //0x44C
    _float32_MSB_LSB  I_rms_Offset;                 //0x44E
    _float32_MSB_LSB  p_V_peak_Offset;
    _float32_MSB_LSB  n_V_peak_Offset;
    _float32_MSB_LSB  p_I_peak_Offset;
    _float32_MSB_LSB  n_I_peak_Offset;

    _float32_MSB_LSB  IREF_to_DAC_Scale;
    _float32_MSB_LSB  IREF_to_DAC_Offset;

    _float32_MSB_LSB ADC_to_EXTIN_L1_Scale;         //0x450
    _float32_MSB_LSB ADC_to_EXTIN_L1_offset;        //0x452


}CALIBRATION_VARIABLES_REG;



//----------------------------------------------------
//
// 校正步驟
// 0x94D
struct CALIBRATION_STEP_BITS {    // bits
    Uint16 Step:8;      // 0-7
    Uint16 Item:3;      // 8-10
};

union CALIBRATION_STEP_REG {
   Uint16                 all;
   struct CALIBRATION_STEP_BITS bit;
};


//----------------------------------------------------
//
// 0x041F Calibration State
//
struct  CALIBRATION_STATE_FLAG_BITS {   // bit
    Uint16 Finish:1;    // 0
    Uint16 Fail:1;      // 1
};
union CALIBRATION_STATE_FLAG_REG {
   Uint16                                   all;
   struct CALIBRATION_STATE_FLAG_BITS   bit;
};



//----------------------------------------------------
//
// SEQ Control
//
struct SEQ_CONTROL_BITS {
    Uint16 Start:1;             // 0x96E
    Uint16 Stop:1;              // 0x96F
    Uint16 Hold:1;              // 0x970
    Uint16 Branch1:1;           // 0x971
    Uint16 Branch2:1;           // 0x972

};

union SEQ_CONTROL_REG {
   Uint16                    all;
   struct SEQ_CONTROL_BITS bit;
};


//----------------------------------------------------
// SEQ Output Mode
//#define SEQ_AC_MODE     0
//#define SEQ_DC_MODE     1
//#define SEQ_ACDC_MODE   2
//
struct SEQ_OUTPUT_MODE_FLAG_BITS {    // bits
    Uint16 rsv01:1;        // 0
    Uint16 rsv02:1;        // 1
    Uint16 rsv03:1;        // 2
};

union SEQ_OUTPUT_MODE_FLAG_REG {
   Uint16                 all;
   struct SEQ_OUTPUT_MODE_FLAG_BITS bit;
};




#define SEQ_STEP_MAX            0x400
//----------------------------------------------------
//
// SEQ參數
//
typedef struct  SEQ_FUNCTION_VARIABLES_REG {

    Uint16 SEQ_Current_Run_Step;    // 0x426
    Uint16 SEQ_Current_State;       // 0x427
    Uint16 SEQ_Current_Set_Step;    // Read:0x428; Write:0x960


    union SEQ_CONTROL_REG Control_flags;

    union SEQ_OUTPUT_MODE_FLAG_REG Output_Mode_set_user;  // 0x973
    union SEQ_OUTPUT_MODE_FLAG_REG Output_Mode_set_user_shadow;  // 0x973

    Uint16 SEQ_Step_Time_set_user_LSB_buf; //for cmd
    Uint16 SEQ_Freq_set_user_LSB_buf; //for cmd
    Uint16 SEQ_DCV_set_user_LSB_buf; //for cmd
    Uint16 SEQ_ACV_set_user_LSB_buf; //for cmd

    float Current_DCV_Set;
    float Current_ACV_Set;
    float Current_Freq_Set;

    float Current_DCV_Set_accu;
    float Current_ACV_Set_accu;
    float Current_Freq_Set_accu;

    Uint32 Current_Step_Count;
    float Current_inv_Step_Count;

    Uint32 Current_Start_Phase_Count;
    Uint32 Current_Stop_Phase_Count;

    Uint16 Current_Jump_Count[SEQ_STEP_MAX];

    Uint16 SEQ_CheckSum; //0x425

    //
    // 0x908(暫定)
    // 0:正常模式
    //   Pin 5 => SYNC 00
    //   Pin 6 => SYNC 01
    // 1:EXTIO 擴展模式
    //   Pin 5 => SYNC 00
    //   Pin 6 => SYNC 01
    //   Pin 1 => SYNC 02
    //   Pin 2 => SYNC 03
    //   Pin 3 => SYNC 04
    //   Pin 4 => SYNC 05
    //   Pin 7 => SYNC 06
    //   Pin 8 => SYNC 07
    //
    //Uint16 EXTIO_Extend_Mode; //0x908

}SEQ_FUNCTION_VARIABLES_REG;




//----------------------------------------------------
//
// MS Sync Status
// Master-Slave 等待同步狀態
//
struct MS_SYNC_STATUS_BITS {
    Uint16 Slave_Wait_ON_Sync:1;        // 0
    Uint16 Slave_Wait_Change_Sync:1;    // 1
    Uint16 Slave_Wait_OFF_Sync:1;       // 2


};

union MS_SYNC_STATUS_REG {
   Uint16                    all;
   struct MS_SYNC_STATUS_BITS bit;
};

//----------------------------------------------------
//
// MS SPIB Sync Flag
//
struct OUTPUT_SYNC_TRIG_BITS {      // bits
    Uint16 Rsv_00:1;                //0
    Uint16 Rsv_01:1;                //1
    Uint16 OFF_Sync_Trig_2:1;       //2
    Uint16 ON_Sync_Trig:1;          //3
    Uint16 Change_Sync_Trig_0:1;    //4
    Uint16 Change_Sync_Trig_1:1;    //5
    Uint16 OFF_Sync_Trig_0:1;       //6
    Uint16 OFF_Sync_Trig_1:1;       //7
};

union OUTPUT_SYNC_TRIG_REG {
   Uint16                 all;
   struct OUTPUT_SYNC_TRIG_BITS bit;
};


//----------------------------------------------------
//
// 外部並聯變數
//
typedef struct  MS_FUNCTION_VARIABLES_REG {


    union POWER_STATUS_REG MS_Power_Status[3];


    union POWER_FUNCTIONS_ERROR_FLAG_REG MS_POWER_FUNCTIONS_ERROR_FLAGS[3];

    union POWER_FUNCTIONS_ERROR_FLAG2_REG MS_POWER_FUNCTIONS_ERROR_FLAGS2[3];

    POWER_DISPLAY_VARIABLES_REG MS_POWER_DISPLAY_VARIABLES[3];

    POWER_DISPLAY_VARIABLES_REG MS_POWER_DISPLAY_VARIABLES_Total;


    union MS_SYNC_STATUS_REG MS_Sync_Status_Flags[3];

    union OUTPUT_SYNC_TRIG_REG Output_Sync_Trig;

    //
    // (ASR-6000)
    // 0x0000: Master
    // 0x0001: Slave 01
    // 0x0002: Slave 02
    // 0x00FF: No Use
    //
    Uint16 MS_Address; // Parallel_Status

    //
    // (ASR-6000)
    // Output Phase
    // 0x0000: 3P4W (3 Units)
    // 0x0001: 1P2W (2 Units)
    // 0x0002: 1P3W (2 Units)
    // 0x0003: 1P2W (3 Units)
    //
    Uint16 MS_Output_Phase;

    Uint16 MS_Total_Units;

    float Master_IFB;

    Uint16 MS_3P4W_Phase_set_user[3];

    Uint16 MS_1P3W_Phase_set_user[2];

    Uint32 MS_Output_Phase_Delay_Count;

}MS_FUNCTION_VARIABLES_REG;




//----------------------------------------------------
typedef struct GLOBAL_VARIABLE_REG{
        MACHINE_SERIES_IMFORMATION_VARIABLES_REG MACHINE_SERIES_IMFORMATION;
        union INPUT_FLAG_REG INPUT_FLAGS;
        union OUTPUT_FLAG_REG OUTPUT_FLAGS;
        union EXT_INPUT_FLAG_REG EXT_INPUT_FLAG;
        union EXT_INPUT_FLAG_REG EXT_INPUT_FLAG_SHADOW;
        union EXT_OUTPUT_FLAG_REG EXT_OUTPUT_FLAG;
        union POWER_CONTROL_REG POWER_CONTROL;
        union POWER_STATUS_REG POWER_STATUS;
        INPUT_VARIABLES_REG INPUT_VARIABLES;
        EXT_INPUT_VARIABLES_REG EXT_INPUT_VARIABLES;
        OUTPUT_VARIABLES_REG OUTPUT_VARIABLES;
        POWER_OUTPUT_SETTING_VARIABLES_REG POWER_OUTPUT_SETTING_VARIABLES;
        POWER_DISPLAY_VARIABLES_REG POWER_DISPLAY_VARIABLES;
        POWER_PROTECT_SETTING_VARIABLES_REG sProtectSetting;
        union POWER_FUNCTIONS_ERROR_FLAG_REG fgError1;
        union POWER_FUNCTIONS_ERROR_FLAG2_REG fgError2;
        union POWER_FUNCTIONS_ENABLE_FLAG_REG POWER_FUNCTIONS_ENABLE_FLAGS;

        Uint16 POWER_FUNCTIONS_ERROR_CODE;

        ///---------0813
        union RD_Mode_IO_Control_REG RD_MODE;

        union VI_RAW_OP_CODE_REG VI_RAW_OP_CODE;
        PRE_CALC_VARIABLES_REG Pre_Calc_Variables;

        CALIBRATION_VARIABLES_REG regCaliData;
        CALIBRATION_VARIABLES_REG regCaliShahow1[2]; // USB存取
        CALIBRATION_VARIABLES_REG regCaliShadow2[2]; // H,L Range

        Uint16 OUTPUT_MODE_set_user;
        union OUTPUT_MODE_FLAG_REG OUTPUT_MODE_FLAG;
        union OUTPUT_MODE_FLAG_REG OUTPUT_MODE_FLAG_Shadow;

        //Uint16 OUTPUT_MODE_FLAG_Shadow;
        union STARTUP_STATE_FLAG_REG fgStartUp;
        union CPU_STATE_FLAG_REG CPU_State_Flag;
        union CALIBRATION_STEP_REG Calibration_Step_Flag;
        union CALIBRATION_STATE_FLAG_REG Calibration_State_Flag;

        union POWER_FUNCTIONS_ENABLE_FLAG2_REG POWER_FUNCTIONS_ENABLE_FLAGS2;

        SEQ_FUNCTION_VARIABLES_REG SEQ_FUNCTION_VARIABLES;

        MS_FUNCTION_VARIABLES_REG MS_Function_Variables;

        float Debug;

        DDSDATA dds;
        DDSDATA dds_shadow;
        DDSDATA dds_shadow2;
        DDSDATA dds_shadow3;

        DDSDATA dds_to_slave;

        Uint32  FreqStep;
        Uint32  FreqStep_shadow;


}GLOBAL_VARIABLE_REG;


extern  volatile GLOBAL_VARIABLE_REG sGB;

#define OUTPUT_RELAY_OFF()            sGB.OUTPUT_FLAGS.bit.OUT_ON = 0;
#define OUTPUT_RELAY_ON()             sGB.OUTPUT_FLAGS.bit.OUT_ON = 1;


void Init_Global_Variables(void);

extern Uint16 isEepromFree(void);
extern Uint16 callEepromData(void);



//
// End of file
//
