#ifndef __RTC_H
#define __RTC_H

#include <stdint.h>

	void RTC_init(void);
	void setTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds, uint8_t AM_PM, uint8_t DST, uint8_t set);
	void setDate(uint8_t day, uint8_t month, uint16_t year, uint8_t weekday);
	void getTime(void);
	void getDate(void);
	void printTime(void);
	void printDate(void);
#endif //__RTC_H
