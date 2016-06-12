#ifndef _CRC16_H_
#define _CRC16_H_

void CRC16_InitChecksum(unsigned short &crcvalue);
void CRC16_UpdateChecksum(unsigned short &crcvalue, const void *data, int length);
void CRC16_FinishChecksum(unsigned short &crcvalue);
unsigned short CRC16_BlockChecksum(const void *data, int length);

#endif
