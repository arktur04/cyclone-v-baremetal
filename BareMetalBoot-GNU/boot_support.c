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
#include "alt_address_space.h"
#include "alt_globaltmr.h"
#include "alt_interrupt.h"
#include "alt_mmu.h"
#include "alt_timers.h"
#include "bridge_support.h"
#include "cache_support.h"
#include "fpga_support.h"
#include "gpio_support.h"
#include "mmu_support.h"
#include "pit_support.h"
#include "uart0_support.h"
#include "boot_support.h"

//
// boot milestone variables
//
uint64_t g_initial_GT_value = 0;
uint64_t g_post_frequency_capture_milestone = 0;
uint64_t g_post_initial_remapping_milestone = 0;
uint64_t g_post_interrupt_system_init_milestone = 0;
uint64_t g_post_mmu_init_milestone = 0;
uint64_t g_post_cache_init_milestone = 0;
uint64_t g_post_interrupts_enabled_milestone = 0;
uint64_t g_post_pit_start_milestone = 0;
uint64_t g_post_pit_enabled_milestone = 0;
uint64_t g_post_gpio_init_milestone = 0;
uint64_t g_post_uart0_init_milestone = 0;
uint64_t g_post_fpga_config_milestone = 0;
uint64_t g_post_last_remapping_milestone = 0;
uint64_t g_post_bridge_init_milestone = 0;
uint64_t g_boot_complete_milestone = 0;

//
// initial frequency variables
//
alt_freq_t g_ALT_CLK_MPU_frequency = 0;
alt_freq_t g_ALT_CLK_DDR_DQS_frequency = 0;
alt_freq_t g_ALT_CLK_MPU_PERIPH_frequency = 0;

//
// boot message log buffer
//
#if (SUPPRESS_BOOT_MESSAGES == 0)
char g_boot_messages_buffer[BOOT_MESSAGE_BUFFER_SIZE] = {0};
#else
char g_boot_messages_buffer[] = "\r\nBoot messages were suppressed.\r\n";
#endif
char *g_boot_messages_end = g_boot_messages_buffer;

int boot_init(void) {

	ALT_STATUS_CODE status;

	//
	// disable interrupts to this cpu since the preloader will enable ECC
	// interrupts by default for the ghrd hardware system that we are using in
	// this example.  If we don't pre-clean the memory that we may touch, we
	// are vulnerable to an ECC interrupt that would lock us up.  So we just
	// disable them.
	//
	status = alt_int_cpu_disable();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("\r\nDisabled interrupts because preloader enabled ECC interrupts.\r\n", g_initial_GT_value);

	//
	// grab the Global Timer value so we have a time stamp for when we started
	// execution of main()
	//
	BOOT_MILESTONE(g_initial_GT_value);
	if(g_initial_GT_value == 0) {
		BOOT_MESSAGE("Global Timer is not running, initializing Global Timer.\r\n", g_initial_GT_value);
		status = alt_globaltmr_init();
		BOOT_CHECK_STATUS;
	}
	BOOT_MESSAGE("Global Timer value at startup = 0x%016llX\r\n", g_initial_GT_value);

	//
	// configure the external clock frequencies for OSC1 and OSC2 so that the
	// clock_manager routines can be used to report accurate frequencies
	//
	status = alt_clk_ext_clk_freq_set(ALT_CLK_IN_PIN_OSC1, MY_OSC1_PIN_FREQ);
	BOOT_CHECK_STATUS;
	status = alt_clk_ext_clk_freq_set(ALT_CLK_IN_PIN_OSC2, MY_OSC2_PIN_FREQ);
	BOOT_CHECK_STATUS;

	//
	// display the frequency of the CPU, DDR controller and the Global Timer
	//
	status = alt_clk_freq_get(ALT_CLK_MPU, &g_ALT_CLK_MPU_frequency);
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("CPU frequency is %lu hertz.\r\n", g_ALT_CLK_MPU_frequency);

	status = alt_clk_freq_get(ALT_CLK_DDR_DQS, &g_ALT_CLK_DDR_DQS_frequency);
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("DDR frequency is %lu hertz.\r\n", g_ALT_CLK_DDR_DQS_frequency);

	status = alt_clk_freq_get(ALT_CLK_MPU_PERIPH, &g_ALT_CLK_MPU_PERIPH_frequency);
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("MPU peripheral frequency is %lu hertz.\r\n", g_ALT_CLK_MPU_PERIPH_frequency);

	BOOT_MILESTONE(g_post_frequency_capture_milestone);

	//
	// remap SDRAM to the ZEROth MB of the MPU address map
	//
	status = alt_mpu_addr_space_remap_0_to_sdram();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("MB ZERO remapped to SDRAM.\r\n");

	//
	// remap the L3 interconnect to mark H2F and LWH2F bridges as inaccessible
	// if the FPGA is known to be programmed already then this may be
	// inappropriate, but this should be the safer thing to do at this point
	//
	status = alt_addr_space_remap(
			ALT_ADDR_SPACE_MPU_ZERO_AT_BOOTROM,
			ALT_ADDR_SPACE_NONMPU_ZERO_AT_SDRAM,
			ALT_ADDR_SPACE_H2F_INACCESSIBLE,
			ALT_ADDR_SPACE_LWH2F_INACCESSIBLE);
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("L3 interconnect remapped as inaccessible and SDRAM.\r\n");

	BOOT_MILESTONE(g_post_initial_remapping_milestone);

	//
	// initialize the interrupt controller system API
	//
	status = alt_int_global_init();
	BOOT_CHECK_STATUS;

	status = alt_int_cpu_init();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("Interrupt controller system API initialized.\r\n");

	BOOT_MILESTONE(g_post_interrupt_system_init_milestone);

	//
	// initialize and enable the MMU
	//
	status = mmu_init();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("MMU initialized and enabled.\r\n");

	BOOT_MILESTONE(g_post_mmu_init_milestone);

	//
	// initialize and enable the caches
	//
	status = cache_init();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("L1 and L2 cache system initialized and enabled.\r\n");

	BOOT_MILESTONE(g_post_cache_init_milestone);

	//
	// enable the GIC to pass interrupt events to the CPU
	//
    status = alt_int_cpu_enable();
	BOOT_CHECK_STATUS;

    status = alt_int_global_enable();
	BOOT_CHECK_STATUS;

	BOOT_MESSAGE("GIC interrupts enabled.\r\n");

	BOOT_MILESTONE(g_post_interrupts_enabled_milestone);

	//
	// Start the PIT
	//
	status = pit_init();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("PIT started.\r\n");

	BOOT_MILESTONE(g_post_pit_start_milestone);

	//
	// register and enable the PIT interrupt
	//
	status = pit_interrupt_enable();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("PIT interrupt registered and enabled.\r\n");

	BOOT_MILESTONE(g_post_pit_enabled_milestone);

	//
	// initialize the GPIOs
	//
	status = gpio_init();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("GPIOs initialized.\r\n");

	BOOT_MILESTONE(g_post_gpio_init_milestone);

	//
	// initialize the UART
	//
	status = uart0_init();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("UART0 initialized.\r\n");

	BOOT_MILESTONE(g_post_uart0_init_milestone);

	//
	// configure the FPGA
	//
	//take out the fpga config
//	status = fpga_init();
//	BOOT_CHECK_STATUS;
//	BOOT_MESSAGE("FPGA configured.\r\n");

//	BOOT_MILESTONE(g_post_fpga_config_milestone);

	//
	// remap the L3 interconnect to mark H2F and LWH2F bridges as accessible
	//
	status = alt_addr_space_remap(
			ALT_ADDR_SPACE_MPU_ZERO_AT_BOOTROM,
			ALT_ADDR_SPACE_NONMPU_ZERO_AT_SDRAM,
			ALT_ADDR_SPACE_H2F_ACCESSIBLE,
			ALT_ADDR_SPACE_LWH2F_ACCESSIBLE);
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("L3 interconnect remapped as accessible and SDRAM.\r\n");

	BOOT_MILESTONE(g_post_last_remapping_milestone);

	//
	// initialize the bridges
	//
	status = bridge_init();
	BOOT_CHECK_STATUS;
	BOOT_MESSAGE("Bridges initialized.\r\n");

	BOOT_MILESTONE(g_post_bridge_init_milestone);

	BOOT_MESSAGE("Boot completed.\r\n");

	BOOT_MILESTONE(g_boot_complete_milestone);
	return 0;
}

void boot_check_status(ALT_STATUS_CODE status, char *file, int line) {

	if(status != ALT_E_SUCCESS) {
		BOOT_MESSAGE("ERROR: bad status detected in file %s on line %d\r\n", file, line);
		while(1);
	}
}

#if (SUPPRESS_BOOT_MESSAGES == 0)

void boot_message(const char *fmt, ...) {

	int len = 0;
	uint32_t remaining_buffer_size = (uint32_t)(&g_boot_messages_buffer) + (uint32_t)(BOOT_MESSAGE_BUFFER_SIZE) - (uint32_t)(g_boot_messages_end);

	if(remaining_buffer_size > 0) {

	    va_list  vl;
	    va_start(vl, fmt);
	    len = vsnprintf(g_boot_messages_end, remaining_buffer_size, fmt, vl);
	    va_end(vl);

	    if(len >= remaining_buffer_size) {
	    	g_boot_messages_buffer[BOOT_MESSAGE_BUFFER_SIZE - 1] = '\0';
	    	len = remaining_buffer_size;
	    	BOOT_SEMIHOSTED_PUTS("ERROR: boot message buffer overflow...\r\n");
	    } else {
	    	BOOT_SEMIHOSTED_PUTS(g_boot_messages_end);
	    }

	    g_boot_messages_end += len;

	} else {
    	BOOT_SEMIHOSTED_PUTS("ERROR: boot message buffer overflow...\r\n");
	}
}

#endif
/* md5sum:301dfed8b64b6396c2b89fd230026416 2013-09-28 20:48:16 */
