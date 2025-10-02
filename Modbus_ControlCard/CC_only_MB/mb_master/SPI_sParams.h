/*
 * SPI_sParams.h
 *
 *  Created on: Aug 27, 2025
 *      Author: roger_lin
 */

#ifndef INCLUDE_SPI_SPARAMS_H_
#define INCLUDE_SPI_SPARAMS_H_



typedef union {
    float32_t f32Data;
    uint32_t u32Data;
    uint16_t u16Data[2];

    struct {
        uint32_t b0_pfc_ovp_n:1;
        uint32_t b1_pfc_uvp_n:1;
        uint32_t b2_pfc_ovp_2_n:1;
        uint32_t b3_pfc_uvp_2_n:1;
        uint32_t b4_pfc_otp_n:1;
        uint32_t b5_dc_ovp_n:1;
        uint32_t b6_ac_fail_5_n:1;
        uint32_t b7_fan_fail_1:1;
        uint32_t b8_fan_fail_2:1;
        uint32_t b9_fan_fail_3:1;
        uint32_t b10_fan_fail_4:1;
        uint32_t b11_15_reserved:5;
        uint32_t b16_off_pwm_n:1;
        uint32_t b17_master:1;
        uint32_t b18_SPIA_State:1;
        uint32_t b19_31_reserved:14;
    } i_stat;

    struct {
        uint32_t b0_15_reserved:16;
        uint32_t b16_off_pwm_n:1;
        uint32_t b17_master:1;
        uint32_t b18_SPIA_State:1;
        uint32_t b19_31_reserved:14;
    } o_stat;
} REG_PARAM;

typedef enum {
    _NO_ACTION_FOR_ISTAT =  0x00000000,
    _ISTAT_PFC_OVP       = (0x00000001<<0),
    _ISTAT_PFC_UVP       = (0x00000001<<1),
    _ISTAT_PFC2_OVP      = (0x00000001<<2),
    _ISTAT_PFC2_UVP      = (0x00000001<<3),
    _ISTAT_PFC_OTP       = (0x00000001<<4),
    _MASK_PFC_ERROR      = (_ISTAT_PFC_OVP|_ISTAT_PFC_UVP|_ISTAT_PFC2_OVP|_ISTAT_PFC2_UVP|_ISTAT_PFC_OTP),
    _ISTAT_DC_OVP        = (0x00000001<<5),
    _ISTAT_AC_FAIL       = (0x00000001<<6),
    _ISTAT_FAN1_FAIL     = (0x00000001<<7),
    _ISTAT_FAN2_FAIL     = (0x00000001<<8),
    _ISTAT_FAN3_FAIL     = (0x00000001<<9),
    _ISTAT_FAN4_FAIL     = (0x00000001<<10),
    _MASK_FAN_HALF_FAIL  = (_ISTAT_FAN1_FAIL|_ISTAT_FAN2_FAIL),
    _MASK_FAN_FULL_FAIL  = (_ISTAT_FAN1_FAIL|_ISTAT_FAN2_FAIL|_ISTAT_FAN3_FAIL|_ISTAT_FAN4_FAIL)

}REG_ISTAT;



// Define a struct containing three register parameters
typedef struct {
    REG_PARAM sDin;
    REG_PARAM sDout;
    REG_PARAM sFanCtrl;
    REG_PARAM sIref_P;
    REG_PARAM sIref_N;
    REG_PARAM sTemp_C;
    REG_PARAM sHeartbeats;
} ST_PARAMS;

// Macro to calculate the offset of a member in the struct
#define INIT_PARAM(name) (uintptr_t)(REG_PARAM*)(&((ST_PARAMS*)(0))->name)/sizeof(REG_PARAM)

// Define enum with calculated offsets
typedef enum {
    _ID_DIN        = INIT_PARAM(sDin),
    _ID_DOUT       = INIT_PARAM(sDout),
    _ID_FAN_CTRL   = INIT_PARAM(sFanCtrl),
    _ID_IREF_P     = INIT_PARAM(sIref_P),
    _ID_IREF_N     = INIT_PARAM(sIref_N),
    _ID_TEMP_C     = INIT_PARAM(sTemp_C),
    _ID_HEARTBEAT  = INIT_PARAM(sHeartbeats),
    _END_OF_IDCMD
} ID_CMD;

extern ST_PARAMS sParams;
extern ST_PARAMS sParamsShadow;

#endif /* INCLUDE_SPI_SPARAMS_H_ */
