/*
 * c28param.h
 *
 *  Created on: Mar 19, 2024
 *      Author: cody_chen
 */

#ifndef C28PARAM_H_
#define C28PARAM_H_

#define FG_GET(x, src)   ((src & (x)) == (x))
#define FG_GETn(x, src)  (!FG_GET(x, src))
#define FG_AND(x, src)   (src & (x))
#define FG_SET(x, src)   {src |= (x);}
#define FG_RST(x, src)   {src &= ~(x);}
#define FG_SWTO(x, src)  {src = (x);}

#define GETn_STAT(x, src) FG_GETn(x, src.fgStatus)
#define GET_STAT(x, src) FG_GET(x, src.fgStatus)
#define SET_STAT(x, src) FG_SET(x, src.fgStatus)
#define RST_STAT(x, src) FG_RST(x, src.fgStatus)

#define AND_ERR(x, src)   FG_AND(x, src.fgError)
#define GET_ERR(x, src)   FG_GET(x, src.fgError)
#define SET_ERR(x, src)   FG_SET(x, src.fgError)
#define RST_ERR(x, src)   FG_RST(x, src.fgError)

#define FG_GETPWM(x)   FG_GET(x, sCLA.sPWM.u16CtrlReg)
#define FG_SETPWM(x)   FG_SET(x, sCLA.sPWM.u16CtrlReg)
#define FG_RSTPWM(x)   FG_RST(x, sCLA.sPWM.u16CtrlReg)


enum {
    _CSTAT_NO_ACTION      =  (0x0000),
    _CSTAT_INIT_DRV_PARAM =  (0x0001<<0),
    _CSTAT_INIT_CLA_PARAM =  (0x0001<<1),
    _CSTAT_INIT_PWMADC    =  (0x0001<<2),
    _CSTAT_THREAD_READY   =  (_CSTAT_INIT_DRV_PARAM|_CSTAT_INIT_CLA_PARAM|_CSTAT_INIT_PWMADC),
    _CSTAT_INIT_PARAMS    =  (0x0001<<3),
    _CSTAT_INIT_SUCCESS   =  (_CSTAT_THREAD_READY|_CSTAT_INIT_PARAMS),
    _CSTAT_BROWN_IN       =  (0x0001<<6),
    _CSTAT_ENABLE_PWM     =  (0x0001<<7),
    _CSTAT_OUTPUT_READY   =  (_CSTAT_BROWN_IN|_CSTAT_ENABLE_PWM),
    _CSTAT_CALIBRATION    =  (0x0001<<8)
};

typedef int16_t FG_CLASTAT;

enum {
    _NO_ERROR     = (0x0000),
    _ERR_VO_OVP   = (0x0001<<0),
    _ERR_VIN_UVP  = (0x0001<<1),
    _ERR_IO_OCP   = (0x0001<<2),
    _ERR_VIN_OVP  = (0x0001<<3),
    _ALL_ERROR    = (_ERR_VO_OVP|_ERR_VIN_UVP|_ERR_IO_OCP|_ERR_VIN_OVP),
    _WARNING_VO   = (0x0001<<8),
    _WARNING_VIN  = (0x0001<<9),
    _WARNING_IO   = (0x0001<<10),
    _ALL_WARNING  = (_WARNING_VO|_WARNING_VIN|_WARNING_IO),
};

typedef uint16_t FG_CLAERR;

typedef volatile struct {

    FG_CLASTAT  fgStatus;
    FG_CLAERR   fgError;

    float32_t f32A;
    float32_t f32B;
    float32_t f32C;
    float32_t f32D;
    float32_t f32E;

}ST_DRV;

extern ST_DRV sDrv;

#endif /* C28PARAM_H_ */
