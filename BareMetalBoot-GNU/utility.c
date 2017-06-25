
#define ALT_HPS_ADDR 0


#include <stdint.h>
#include "stdarg.h"
#include <stdio.h>
#include "alt_clock_manager.h"
#include "socal/alt_rstmgr.h"
#include "socal/socal.h"
#include "socal/alt_uart.h"
#include "alt_16550_uart.h"
#include "socal/alt_l4wd.h"
#include "socal/hps.h"
//#include "alt_os_abstract_layer.h"

//void delay_us(int us);

/* Print character to UART */
int uart_put_char(char value)
{
    uint32_t reg;
    /* Wait until THR is empty*/
    do
    {
       reg = alt_read_word(ALT_UART0_LSR_ADDR);
//	   delay_us(100);
    } while(ALT_UART_LSR_THRE_GET(reg) != 1);

    /* Write character to THR */
    alt_write_word(ALT_UART0_RBR_THR_DLL_ADDR, value);
    return (reg);
}


/* TODO: use global timer and put a real delay here, like in the I2C example program */
void delay_us(int us)
{
	volatile int i;
	for(i=0; i < (us*3); i++)
	{
		__asm(" nop ");
	}
}

void watchdog_kick(void)
{
	/* kick the watchdog */
	alt_write_word(ALT_L4WD0_WDT_CRR_ADDR, ALT_L4WD_CRR_WDT_CRR_E_KICK);
}
