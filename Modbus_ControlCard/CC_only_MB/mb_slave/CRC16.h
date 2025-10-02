
#ifndef CRC16_H_
#define CRC16_H_

#include <stdint.h>

typedef struct {
	uint16_t	wCRCWord;
	unsigned int  	usCRCindex;
} ST_CRCs;


typedef ST_CRCs * HAL_CRCs;

#define DEFAULT_CRCs (ST_CRCs){0xFFFF, 0}

extern uint16_t ucMBCRC16(uint16_t data, HAL_CRCs ref);



#endif /* CRC16_H_ */
