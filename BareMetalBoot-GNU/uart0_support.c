/******************************************************************************
*
* Copyright 2013 Altera Corporation. All Rights Reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. The name of the author may not be used to endorse or promote products
* derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED. IN NO
* EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
******************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "socal/socal.h"
#include "hwlib.h"
#include "alt_16550_uart.h"
#include "uart0_support.h"
#include "alt_interrupt_common.h"
#include "alt_interrupt.h"
//#include "alt_interrupt_armcc.s"
//#include "boot_support.h"

// Interrupt attribute(s) flags

#define INT_MSG	"/n/rInterrupt /n/r\0"
#define INIT_MSG	"/n/rUart Init /n/r\0"
#define	ALT_OSAL_UART_DEFAULT_IRQ_PRIORITY			16

//static ALT_16550_HANDLE_t g_uart0_handle;
ALT_16550_HANDLE_t g_uart0_handle;

// Interrupt dispatch information
// See Cortex-A9 MPCore TRM, section 1.3.
// SGI (16) + PPI (16) + SPI count (224) = total number of interrupts.
typedef struct INT_DISPATCH_s
{
    alt_int_callback_t callback;
    void *             context;
}
INT_DISPATCH_t;


typedef enum INT_FLAG_e
{
    INT_FLAG_IMPLEMENTED = 0x01
}
INT_FLAG_t;



//ALT_STATUS_CODE alt_int_isr_register(ALT_INT_INTERRUPT_t int_id,
//                                     alt_int_callback_t callback,
//                                     void * context);


inline static uint32_t get_current_cpu_num(void);

void os_abstract_layer_uart_irq_callback(uint32_t icciar, void * context);

inline static void set_sctlr_vbit(bool value);

inline static uint32_t get_periphbase(void);

/***************************************************************************************************************/
/*ALT_STATUS_CODE alt_int_global_init()
{
    // Cache the distributor and CPU base addresses
    // See: Cortex-A9 MPCore TRM, section 1.5.
    {
        uint32_t periphbase = get_periphbase();
        alt_int_base_dist = periphbase + 0x1000;
        alt_int_base_cpu  = periphbase + 0x0100;
    }

    // Discover CPU and interrupt count
    // See GIC 1.0, section 4.3.2.
    {
        uint32_t icdictr = alt_read_word(alt_int_base_dist + 0x4);
        alt_int_count_cpu = ((icdictr >> 5) & 0x7) + 1;
        alt_int_count_int = ((icdictr & 0x1F) + 1) << 5;
    }

    // Initialize the callback and context array
    // Initialize interrupt flags array
    for (int i = 0; i < ALT_INT_PROVISION_INT_COUNT; ++i)
    {
        alt_int_dispatch[i].callback = 0;
        alt_int_dispatch[i].context  = 0;

        alt_int_flag[i] = 0;
    }

    // Discover all interrupts that are implemented in hardware.
    // See GIC 1.0, section 3.1.2.
    for (int i = 0; i < (ALT_INT_PROVISION_INT_COUNT / 32); ++i)
    {
        // Set the whole bank to be enabled.
        alt_write_word(alt_int_base_dist + 0x100 + i * sizeof(uint32_t), 0xffffffff); // icdisern

        // Read it back to see which bits stick.
        uint32_t value = alt_read_word(alt_int_base_dist + 0x100 + i * sizeof(uint32_t)); // icdisern
        for (int j = 0; j < 32; ++j)
        {
            if (((1 << j) & value) != 0)
            {
                alt_int_flag[i * 32 + j] |= INT_FLAG_IMPLEMENTED;
            }
        }

        // Clear the whole bank to be disabled.
        alt_write_word(alt_int_base_dist + 0x180 + i * sizeof(uint32_t), 0xffffffff); // icdicern
    }

    return ALT_E_SUCCESS;
}
*/
/********************************************************************************************************************/
inline static uint32_t get_periphbase(void)
{
    uint32_t periphbase;

    // Read the Peripheral Base Address.
    // See: Cortex-A9 TRM, section 4.3.24.

#ifdef __ARMCC_VERSION
    __asm("MRC p15, 4, periphbase, c15, c0, 0");
#else
    __asm("MRC p15, 4, %0, c15, c0, 0" : "=r" (periphbase));
#endif

    return periphbase;
}





/***********************************************************************************************************************/
//#if ALT_INT_PROVISION_VECTOR_SUPPORT
inline static void set_sctlr_vbit(bool value)
{
    // See ARMv7, section B4.1.130.

    uint32_t sctlr;

#ifdef __ARMCC_VERSION
    __asm("MRC p15, 0, sctlr, c1, c0, 0");
#else
    __asm("MRC p15, 0, %0, c1, c0, 0" : "=r" (sctlr));
#endif

    if (value)
    {
        sctlr |= 1 << 13;
    }
    else
    {
        sctlr &= ~(1 << 13);
    }

#ifdef __ARMCC_VERSION
    __asm("MCR p15, 0, sctlr, c1, c0, 0");
#else
    __asm("MCR p15, 0, %0, c1, c0, 0" : : "r" (sctlr));
#endif

}
//#endif
#if 0
/***************************************************************************************************************/
/*******************************************************************************
 * os_abstract_layer_create() will initialize the HW and modules required by
 * the OS.
 * This module may be called before or after the kernel is created. However,
 * it MUST BE CALLED before os_abstract_layer_init().
*******************************************************************************/
int32_t	os_abstract_layer_create(void)
{
    int32_t	result =0;

    ////////////////////////////////////////////////////
    // Initialize the Global Interrupt Controller (GIC)
    ////////////////////////////////////////////////////
    // Intialize the registers, clear the interrupt vector array, disable IRQs.
    result |= alt_int_global_init();
    // Make sure the distributor is disabled.
    result = alt_int_global_disable();
    // Initialize IRQ stack, GIC configuration, default priorities.
    result |= alt_int_cpu_init(); // This one messes up ThreadX - probably the stack.
    result |= alt_int_cpu_binary_point_set(0);
    result |= alt_int_cpu_priority_mask_set(255);
    // Enable the GIC CPU Interrupt
    result |= alt_int_cpu_enable();
    // Make sure the CPU interface control register enables INT.
    result |= alt_int_cpu_enable();
    // Enable the GIC Distributor Interrupt.
    result |= alt_int_global_enable();
    return (result);
//    return (result ? ALT_OSAL_FAIL : ALT_OSAL_SUCCESS);
}

/* in freeRTOS this is already done We don't use these calls
 *
 */
#endif
/***************************************************************************************************************/

ALT_STATUS_CODE uart0_init(void) {

	int j=100;

	int i;

	ALT_STATUS_CODE status =1;

    status = alt_16550_init(ALT_16550_DEVICE_SOCFPGA_UART0, 0, 0, &g_uart0_handle);
 //   BOOT_CHECK_STATUS;

    status |= alt_16550_line_config_set(&g_uart0_handle, ALT_16550_DATABITS_8, ALT_16550_PARITY_DISABLE, ALT_16550_STOPBITS_1);
 //   BOOT_CHECK_STATUS;

    status |= alt_16550_baudrate_set(&g_uart0_handle, ALT_16550_BAUDRATE_115200);
//    status = alt_16550_baudrate_set(&g_uart0_handle, ALT_16550_BAUDRATE_57600);
  //  BOOT_CHECK_STATUS;

    status |= alt_16550_fifo_enable(&g_uart0_handle);
  //  BOOT_CHECK_STATUS;

    status |= alt_16550_enable(&g_uart0_handle);
  //  BOOT_CHECK_STATUS;


//#ifdef ALT_TX_USE_UART_IRQ
	// Disable the Local UART interrupt.
	status |= alt_16550_int_disable_rx(&g_uart0_handle);
	// Set the callback for the interrupt function.
    status |= alt_int_isr_register( ALT_INT_INTERRUPT_UART0,
    						&os_abstract_layer_uart_irq_callback,
    						NULL);

	////////////////////////////////////////////////////
    // Setup the UART Interrupt
    ////////////////////////////////////////////////////
	// Configure the UART as level.
//	status |= alt_int_dist_trigger_set(ALT_INT_INTERRUPT_UART0,
//                             			ALT_INT_TRIGGER_LEVEL);
    // Configure the UART priority
//    status |= alt_int_dist_priority_set(ALT_INT_INTERRUPT_UART0,
//     						ALT_OSAL_UART_DEFAULT_IRQ_PRIORITY);
    // Set both CPUs as the target.
//     status |= alt_int_dist_target_set(ALT_INT_INTERRUPT_UART0,
//    									0x03);
    // Set First CPUs as the target.
//     status |= alt_int_dist_target_set(ALT_INT_INTERRUPT_UART0,
//    									0x01);
     // Set second CPUs as the target.
//      status |= alt_int_dist_target_set(ALT_INT_INTERRUPT_UART0,
//     									0x02);
	// Enable the interrupt in the Distributor.
//     status |= alt_int_dist_enable(ALT_INT_INTERRUPT_UART0);                     //

//    status |= alt_int_dist_is_enabled(ALT_INT_INTERRUPT_UART0);

    // Enable the Local UART interrupt.
// 	status |= alt_16550_int_enable_rx(&g_uart0_handle);

	// Since the UART is so integral to this work, if it
	// can't be initialized, make it fatal.
//	if (status) while(1);

	for(i=0; i < j; i++)
	{
		__asm(" nop ");
	}


	return (status);
}
/*********************************************************************************************************************/
void set_uart0_int(void)
{
	ALT_STATUS_CODE status =1;
	////////////////////////////////////////////////////
    // Setup the UART Interrupt
    ////////////////////////////////////////////////////
	// Configure the UART as level.
	status |= alt_int_dist_trigger_set(ALT_INT_INTERRUPT_UART0,
                             			ALT_INT_TRIGGER_LEVEL);
    // Configure the UART priority
    status |= alt_int_dist_priority_set(ALT_INT_INTERRUPT_UART0,
     						ALT_OSAL_UART_DEFAULT_IRQ_PRIORITY);
    // Set both CPUs as the target.
//     status |= alt_int_dist_target_set(ALT_INT_INTERRUPT_UART0,
//    									0x03);
    // Set First CPUs as the target.
     status |= alt_int_dist_target_set(ALT_INT_INTERRUPT_UART0,
    									0x01);
     // Set second CPUs as the target.
//      status |= alt_int_dist_target_set(ALT_INT_INTERRUPT_UART0,
//     									0x02);
	// Enable the interrupt in the Distributor.
     status |= alt_int_dist_enable(ALT_INT_INTERRUPT_UART0);                     //

    status |= alt_int_dist_is_enabled(ALT_INT_INTERRUPT_UART0);

    // Enable the Local UART interrupt.
 	status |= alt_16550_int_enable_rx(&g_uart0_handle);

	// Since the UART is so integral to this work, if it
	// can't be initialized, make it fatal.
//	if (status) while(1);



}


/**********************************************************************************************************************/

ALT_STATUS_CODE uart0_uninit(void) {

	ALT_STATUS_CODE status;

	status = alt_16550_disable(&g_uart0_handle);
//    BOOT_CHECK_STATUS;

	status = alt_16550_fifo_disable(&g_uart0_handle);
//    BOOT_CHECK_STATUS;

    status = alt_16550_uninit(&g_uart0_handle);
//    BOOT_CHECK_STATUS;

	return status;
}
/************************************************************************************************************************/
ALT_STATUS_CODE uart0_print(const char *in_str) {

	ALT_STATUS_CODE status = ALT_E_SUCCESS;

    int len = strlen(in_str);

	uint32_t size_tx;
	if (status == ALT_E_SUCCESS) {
		status = alt_16550_fifo_size_get_tx(&g_uart0_handle, &size_tx);
	}

	for (int i = 0; i < 1000; ++i)
	{
		if (status != ALT_E_SUCCESS) {
			break;
		}

		if (len == 0) {
			break;
		}

		// Wait for the THRE line status
		int j = 1000000;
		while (--j) {
			uint32_t line_status = 0;
			status = alt_16550_line_status_get(&g_uart0_handle, &line_status);
			if (status != ALT_E_SUCCESS) {
				break;
			}
			if (line_status & (ALT_16550_LINE_STATUS_THRE | ALT_16550_LINE_STATUS_TEMT)) {
				break;
			}
		}
		if (j == 0) {
			status = ALT_E_TMO;
		}

		uint32_t level_tx;

		if (status == ALT_E_SUCCESS) {
			status = alt_16550_fifo_level_get_tx(&g_uart0_handle, &level_tx);
		}

		if (status == ALT_E_SUCCESS) {
			uint32_t size_write = MIN(len, size_tx - level_tx);
			status = alt_16550_fifo_write(&g_uart0_handle, in_str, size_write);
			if (status == ALT_E_SUCCESS) {
				len -= size_write;
				in_str += size_write;
			}
		}
	}

	return status;
}

/*********************************************************************************************************************/

int uart0_printf(const char *fmt, ...) {

	ALT_STATUS_CODE status = ALT_E_SUCCESS;
	int len = 0;
	char buffer[1024];

	va_list  vl;
	va_start(vl, fmt);
	len = vsnprintf(buffer, sizeof(buffer), fmt, vl);
	va_end(vl);

	if(len >= sizeof(buffer)) {

		buffer[sizeof(buffer) - 1] = '\0';

		status = uart0_print(buffer);
		if( status != ALT_E_SUCCESS)
			return status;

		status = uart0_print("\r\nERROR: uart0_printf_buffer overflow...\r\n");
		if( status != ALT_E_SUCCESS)
			return status;

	} else {
		status = uart0_print(buffer);
		if( status != ALT_E_SUCCESS)
			return status;
	}

	return len;
}

/***************************************************************************************************************************/

int uart0_getc(void) {

	ALT_STATUS_CODE status = ALT_E_SUCCESS;
	int ret_val = EOF;

	uint32_t level;
	status = alt_16550_fifo_level_get_rx(&g_uart0_handle, &level);
	if(status != ALT_E_SUCCESS)
		return ret_val;

	if(level > 0) {
		char buffer;
		status = alt_16550_fifo_read(&g_uart0_handle, &buffer, 1);
		if(status != ALT_E_SUCCESS)
			return ret_val;
		else
			return buffer;
	}

	return ret_val;
}

/**********************************************************************************************************************/

int uart0_putc(int value) {

	ALT_STATUS_CODE status = ALT_E_SUCCESS;
	int ret_val = EOF;
	uint32_t level;
	while ((status != ALT_E_SUCCESS))
	{
	status = alt_16550_fifo_level_get_tx(&g_uart0_handle, &level);
	}
	if(level > 0) {
		char buffer;
		buffer = value;
		status = alt_16550_fifo_write(&g_uart0_handle, &buffer, 1);
		if(status != ALT_E_SUCCESS)
			return ret_val;
		else
			return buffer;
	}

	return ret_val;
}

/***********************************************************************************************************************/

//#ifdef ALT_TX_USE_UART_IRQ
//ALT_INT_INTERRUPT_UART0 = 194, /*!< # */
//ALT_INT_INTERRUPT_UART1 = 195,
/*!<
 * Interrupts sourced from the UARTs 0 - 1.
 *  * All interrupts in this group are level triggered.
 */
/*******************************************************************************
* os_abstract_layer_uart_irq_callback() will handle the
 * UART IRQ callback.
*******************************************************************************/
int g_test = 0;
void os_abstract_layer_uart_irq_callback(uint32_t icciar, void * context)
{
	Int_Char = EOF;
	uint32_t rx_level=0;

	// Check to make sure this is a valid UART interrupt.
	if (ALT_INT_INTERRUPT_UART0 == icciar)
	{
		ALT_16550_INT_STATUS_t irq_status;

		alt_16550_int_status_get(&g_uart0_handle, &irq_status);
		// if we received data, handle it here.
		if (irq_status == ALT_16550_INT_STATUS_RX_DATA)
		{

//	#ifdef ALT_TX_USE_UART_FIFO
			// Normally, there is only 1 value in UART but
			// we may have missed an IRQ so read the # if
			// we are in FIFO mode.
//	#endif		//#ifdef ALT_TX_USE_UART_FIFO

			if(rx_level > 0) {
				char buffer;
				int reg=0;
				reg = alt_16550_fifo_read(&g_uart0_handle, &buffer, 1);
				Int_Char=buffer;
			}
		}
		g_test++;
	}
}
//#endif		//ALT_TX_USE_UART_IRQ

/***********************************************************************************************************************/

inline static uint32_t get_current_cpu_num(void)
{
    uint32_t affinity;

    // Use the MPIDR. This is only available at PL1 or higher.
    // See ARMv7, section B4.1.106.

#ifdef __ARMCC_VERSION
    __asm("MRC p15, 0, affinity, c0, c0, 5");
#else
    __asm ("MRC p15, 0, %0, c0, c0, 5" : "=r" (affinity));
#endif

    return affinity & 0xFF;
}

void scia_msg(char * msg)
{
    int i;
    i = 0;

    while(msg[i] != '\0')
    {
    	uart0_putc(msg[i]);
        i++;
    }
}


/* md5sum:3086bef986334caa60e4e70a63fba834 2013-09-28 20:48:16 */

/***************************************************************************************************************************/
