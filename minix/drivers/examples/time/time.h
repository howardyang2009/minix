#ifndef __TIME_H
#define __TIME_H
 
/* Major ID of /dev/time. */
#define TIME_MAJOR  17
 
/* Base I/O port of the CMOS. */
#define CMOS_PORT   0x70
 
/* Data field offsets of the RTC in CMOS. */
#define RTC_SECONDS      0
#define RTC_MINUTES      2
#define RTC_HOURS        4
#define RTC_DAY_OF_WEEK  6
#define RTC_DAY_OF_MONTH 7
#define RTC_MONTH        8
#define RTC_YEAR         9
 
/* RTC Registers and Flags. */
#define RTC_STATUS_A     10
#define RTC_STATUS_B     11
#define RTC_UIP          0x80
#define RTC_BCD          0x04
 
#endif /* __TIME_H */