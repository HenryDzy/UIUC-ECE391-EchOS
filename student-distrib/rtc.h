#ifndef _RTC_H
#define _RTC_H
#include "types.h"

#define RTC_PORT 0x70       // specify an index or "register number", and to disable NMI.
#define RTC_PORT_CMOS 0X71  // read or write from/to that byte of CMOS configuration space
#define RTC_IRQ_NUM 8
#define RTC_REG_A 0x8A
#define RTC_REG_B 0X8B
#define RTC_REG_C 0X8C


void RTC_init();
void handler_rtc();
int32_t rtc_write(int32_t fd, const void* buf,int32_t nbytes);
int32_t rtc_read(int32_t fd,void* buf,int32_t nbytes);
int32_t rtc_open(const uint8_t* filename);
int32_t rtc_close(int32_t fd);
#endif
