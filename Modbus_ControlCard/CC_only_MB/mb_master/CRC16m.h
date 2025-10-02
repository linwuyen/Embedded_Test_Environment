
#ifndef CRC16m_H_
#define CRC16m_H_


#include <stdint.h>

typedef struct {
	unsigned char	ucCRCHi;
	unsigned char	ucCRCLo;


	unsigned int  	usCRCindex;
} ST_CRC;


typedef ST_CRC * HAL_CRC;

#define DEFAULT_CRC (ST_CRC){0xFF, 0xFF, 0}

extern uint16_t ucMBCRC16m(unsigned int data, HAL_CRC ref);
//extern unsigned int ucBLOCKCRC16(unsigned int *src, unsigned int len, HAL_CRC ref);
//extern unsigned int setCRC16(unsigned int data, ST_CRC *ref);
//extern unsigned int getCRC16(ST_CRC *ref);
//extern unsigned int getCRC16n(ST_CRC *ref);

#endif /* CRC16_H_ */
