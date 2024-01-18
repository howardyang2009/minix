#include <stdio.h>
#include <stdlib.h>
#include <minix/ds.h>
#include <minix/chardriver.h>
 
#include "time.h"
 
/* Function prototypes for the time driver. */
int time_open(devminor_t minor, int access, endpoint_t user_endpt);
int time_close(devminor_t minor);
int time_read  (devminor_t minor, u64_t position, endpoint_t endpt,
        cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);
 
int cmos_read_byte (int offset);
unsigned bcd_to_bin (unsigned value);
void time_from_cmos (char *buffer, int size);
 
/* SEF functions and variables. */
void sef_local_startup (void);
int sef_cb_init_fresh (int type, sef_init_info_t *info);
 
/* Entry points to the time driver. */
struct chardriver time_tab =
{
        .cdr_open = time_open,
        .cdr_close = time_close,
        .cdr_read = time_read,
};
 
int time_open(devminor_t minor, int access, endpoint_t user_endpt)
{
    printf("time_open()\r\n");
    return OK;
}
 
int time_close(devminor_t minor)
{
    printf("time_close()\r\n");
    return OK;
}
 
int cmos_read_byte(int offset)
{
    uint32_t value = 0;
    int r;
 
    if ((r = sys_outb(CMOS_PORT, offset)) != OK)
    {
        panic("sys_outb failed: %d", r);
    }
    if ((r = sys_inb(CMOS_PORT + 1, &value)) != OK)
    {
        panic("sys_inb failed: %d", r);
    }
    return value;
}
 
unsigned bcd_to_bin(unsigned value)
{
    return (value & 0x0f) + ((value >> 4) * 10);
}
 
void time_from_cmos(char *buffer, int size)
{
    int sec, min, hour, day, mon, year;
 
    /*
          * Wait until the Update In Progress (UIP) flag is clear, meaning
          * that the RTC registers are in a stable state.
          */
    while (!(cmos_read_byte(RTC_STATUS_A) & RTC_UIP))
    {
        ;
    }
    /* Read out the time from RTC fields in the CMOS. */
    sec  = cmos_read_byte(RTC_SECONDS);
    min  = cmos_read_byte(RTC_MINUTES);
    hour = cmos_read_byte(RTC_HOURS);
    day  = cmos_read_byte(RTC_DAY_OF_MONTH);
    mon  = cmos_read_byte(RTC_MONTH);
    year = cmos_read_byte(RTC_YEAR);
 
    /* Convert from Binary Coded Decimal (BCD), if needed. */
    if (cmos_read_byte(RTC_STATUS_B) & RTC_BCD)
    {
        sec  = bcd_to_bin(sec);
        min  = bcd_to_bin(min);
        hour = bcd_to_bin(hour);
        day  = bcd_to_bin(day);
        mon  = bcd_to_bin(mon);
        year = bcd_to_bin(year);
    }
    /* Convert to a string. */
    snprintf(buffer, size, "%04d-%02x-%02x %02x:%02x:%02x\r\n",
             year + 2000, mon, day, hour, min, sec);
}
 
ssize_t time_read(devminor_t minor, u64_t position, endpoint_t endpt,
        cp_grant_id_t grant, size_t size, int flags, cdev_id_t id)
{
    int bytes, ret;
    char buffer[1024];
 
    printf("time_read()\r\n");
 
    /* Retrieve system time from CMOS. */
    time_from_cmos(buffer, sizeof(buffer));
 
    bytes = MIN(strlen(buffer) - (int) position, size);
 
    if (bytes <= 0)
    {
        return OK;
    }
 
    ret = sys_safecopyto(endpt, grant, 0, (vir_bytes) buffer + position, bytes);
 
    if(ret != OK) return 0;
 
    return bytes;
}
 
void sef_local_startup()
{
    /* Register init callbacks. */
    sef_setcb_init_fresh(sef_cb_init_fresh);
    sef_setcb_init_lu(sef_cb_init_fresh);      /* treat live updates as fresh inits */
    sef_setcb_init_restart(sef_cb_init_fresh); /* treat restarts as fresh inits */
 
    /* Register live update callbacks. */
    sef_setcb_lu_prepare(sef_cb_lu_prepare_always_ready);         /* agree to update immediately when a LU request is received in a supported state */
    sef_setcb_lu_state_isvalid(sef_cb_lu_state_isvalid_standard); /* support live update starting from any standard state */
 
    /* Let SEF perform startup. */
    sef_startup();
}
 
int sef_cb_init_fresh(int type, sef_init_info_t *info)
{
    /* Initialize the time driver. */
    return(OK);
}
 
int main(int argc, char **argv)
{
    /* Perform initialization. */
    sef_local_startup();
 
    /* Run the main loop. */
    chardriver_task(&time_tab);
    return OK;
}