



#include "utility.h"
#include "uart0_support.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "socal/alt_uart.h"
#include "alt_16550_uart.h"
#include "socal/hps.h"
#include "socal/hps.h"
//#include "alt_os_abstract_layer.h"
#include "socal/alt_l4wd.h"
#include "boot_support.h"
#include "alt_i2c.h"
#include "alt_spi.h"


	int prompt = 0;
	int CmdLen = 0;
	char CmdStr[34];
	int Int_Char= EOF;


//#define printf alt_printf
#define printf uart0_printf

#define INC_CUM_US(a, b) (double)(a - b) / (double)g_ALT_CLK_MPU_PERIPH_frequency * 1.0E6, (double)(a - g_initial_GT_value) / (double)g_ALT_CLK_MPU_PERIPH_frequency * 1.0E6

	void dump_boot_message_buffer(void) {

		ALT_STATUS_CODE status;

		uart0_printf("\r\n");

		status = uart0_print(g_boot_messages_buffer);
		if(status != ALT_E_SUCCESS) {
				uart0_printf("ERROR: in dump_boot_message_buffer() calling uart0_print()\r\n");
			while(1);
		}
	}


	void dump_boot_milestones(void) {

		uart0_printf("\r\n");
		uart0_printf("Boot milestones:\r\n");
		uart0_printf("%18s : %18s : %12s : %12s\r\n", "MILESTONE", "GT TICKCOUNT", "INCREMENTAL", "CUMULATIVE");
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "initial GT",				g_initial_GT_value, 					0.0, 0.0);
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "frequency capture", 		g_post_frequency_capture_milestone,		INC_CUM_US(g_post_frequency_capture_milestone,		g_initial_GT_value));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "initial remapping", 		g_post_initial_remapping_milestone,		INC_CUM_US(g_post_initial_remapping_milestone,		g_post_frequency_capture_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "interrupt sys init",		g_post_interrupt_system_init_milestone,	INC_CUM_US(g_post_interrupt_system_init_milestone,	g_post_initial_remapping_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "mmu init", 				g_post_mmu_init_milestone,				INC_CUM_US(g_post_mmu_init_milestone,				g_post_interrupt_system_init_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "cache init", 				g_post_cache_init_milestone, 			INC_CUM_US(g_post_cache_init_milestone,				g_post_mmu_init_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "interrupts enabled", 		g_post_interrupts_enabled_milestone, 	INC_CUM_US(g_post_interrupts_enabled_milestone,		g_post_cache_init_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "pit start", 				g_post_pit_start_milestone, 			INC_CUM_US(g_post_pit_start_milestone,				g_post_interrupts_enabled_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "pit enabled", 			g_post_pit_enabled_milestone, 			INC_CUM_US(g_post_pit_enabled_milestone,			g_post_pit_start_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "gpio init", 				g_post_gpio_init_milestone, 			INC_CUM_US(g_post_gpio_init_milestone,				g_post_pit_enabled_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "uart0 init", 				g_post_uart0_init_milestone,			INC_CUM_US(g_post_uart0_init_milestone,				g_post_gpio_init_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "fpga config", 			g_post_fpga_config_milestone,			INC_CUM_US(g_post_fpga_config_milestone,			g_post_uart0_init_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "last remapping", 			g_post_last_remapping_milestone,		INC_CUM_US(g_post_last_remapping_milestone,			g_post_fpga_config_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "bridge init", 			g_post_bridge_init_milestone,			INC_CUM_US(g_post_bridge_init_milestone,			g_post_last_remapping_milestone));
		uart0_printf("%18s : 0x%016llX : %10.3fus : %10.3fus\r\n", "boot complete", 			g_boot_complete_milestone,				INC_CUM_US(g_boot_complete_milestone,				g_post_bridge_init_milestone));
	}


int main(void)
{
	int k = 0;
	int c;
	ALT_I2C_DEV_t  i2c_dev;
	ALT_SPI_DEV_t  spi_dev;

	k = boot_init();


	alt_i2c_init(ALT_I2C_I2C0, &i2c_dev);

	alt_spi_init(ALT_SPI_SPIM0, &spi_dev);
	ALT_SPI_CONFIG_t spi_config[]={{ALT_SPI_DFS_8BIT, ALT_SPI_FRF_SPI,ALT_SPI_SCPH_TOGGLE_MIDDLE,ALT_SPI_SCPOL_INACTIVE_LOW,ALT_SPI_TMOD_TXRX,ALT_SPI_OP_MODE_MASTER,0}};
	alt_spi_config_set(&spi_dev, spi_config);
	//	alt_spi_divider_set(&spi_dev, 115200);  // this is the actual divider value.  I believe the clock is 200Mhz max value is 65,534  200,000,000/115200 = 1736
//	or you could use speed_set

	alt_spi_speed_set(&spi_dev, 115200);
	alt_spi_enable(&spi_dev);

	alt_spi_slave_select_enable(&spi_dev ,ALT_SPI_SS0);
	alt_spi_num_data_frames_set	(&spi_dev,2)	;
	alt_spi_slave_select_disable(&spi_dev ,ALT_SPI_SS0);




	//
	// dump the boot message buffer
	//
	dump_boot_message_buffer();

	//
	// dump the boot milestones
	//
	dump_boot_milestones();

	//printf("Hello 4_11_2014 14:00 %d!\n\r", k);
	printf("Hello world, 32bit-me!\n\r");

	while(1)
	{

		k++;
		watchdog_kick();
//		delay_us(1000000);
		if (Int_Char!= EOF)
		{
			c =	Int_Char;
			printf("\n\rInt!\n\r");
		}
		else
		{
		c = uart0_getc();
		}
		if ( c != EOF)
		{
		prompt=c;
//		if (prompt != 0x0D && CmdLen<=31 )
		if (CmdLen<=31 )
		{                             // Get a string of Char

		if( prompt == 0x8 ) {         // Back space ?
			if (CmdLen >=0)
			CmdLen--;
			 {

			    printf("%c",(char)prompt);        //Echo BackSpace if not full duplex
			    printf("%c",(char)0x020);          //Echo Space     if not full duplex
			    printf("%c",(char)prompt);        //Echo BackSpace if not full duplex
			 }
		}
		else {
				CmdStr[CmdLen++] = prompt;
//				printf("%c",(char)prompt);       //Echo RX Char if not full duplex
				uart0_putc(prompt);              //Echo RX Char if not full duplex
				if (prompt == 0x0D)              //If CR
				{
					printf("%c",(char)0x0A);     //Echo LF
					CmdStr[CmdLen++] = 0x0D;
					CmdLen = 0;
			    }
		 } // End else
		} // End if (CmdLen<=31 )

	   } // End if ( c != EOF)
		if (CmdLen>=31 )
		{
					CmdLen = 0;
					printf("\n\rNew buffer !\n\r");
					set_uart0_int();
		}
/*****************************************************************************************************************/
	}  // End while(1)
	return 0;
}








