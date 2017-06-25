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

#ifndef BOOT_SUPPORT_H_
#define BOOT_SUPPORT_H_

#include "alt_clock_manager.h"
#include "alt_globaltmr.h"

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

#define MY_OSC1_PIN_FREQ	(25000000)
#define MY_OSC2_PIN_FREQ	(25000000)

#define BOOT_CHECK_STATUS	boot_check_status(status, __FILE__, __LINE__)

#ifndef SUPPRESS_BOOT_MESSAGES

#error SUPPRESS_BOOT_MESSAGES must be defined

#elif (SUPPRESS_BOOT_MESSAGES == 1)

#define BOOT_MESSAGE(...)

#else

#define BOOT_MESSAGE(fmt, ...)	boot_message(fmt, ##__VA_ARGS__)

#endif

#ifndef SUPPRESS_SEMIHOSTING

#error SUPPRESS_SEMIHOSTING must be defined

#elif (SUPPRESS_SEMIHOSTING == 1)

#define BOOT_SEMIHOSTED_PUTS(str)

#else

#define BOOT_SEMIHOSTED_PUTS(str)	puts(str)

#endif

#define BOOT_MESSAGE_BUFFER_SIZE	(8 * 1024)

#define BOOT_MILESTONE(x)	x = alt_globaltmr_get64()

extern uint64_t g_initial_GT_value;
extern uint64_t g_post_frequency_capture_milestone;
extern uint64_t g_post_initial_remapping_milestone;
extern uint64_t g_post_interrupt_system_init_milestone;
extern uint64_t g_post_mmu_init_milestone;
extern uint64_t g_post_cache_init_milestone;
extern uint64_t g_post_interrupts_enabled_milestone;
extern uint64_t g_post_pit_start_milestone;
extern uint64_t g_post_pit_enabled_milestone;
extern uint64_t g_post_gpio_init_milestone;
extern uint64_t g_post_uart0_init_milestone;
extern uint64_t g_post_fpga_config_milestone;
extern uint64_t g_post_last_remapping_milestone;
extern uint64_t g_post_bridge_init_milestone;
extern uint64_t g_boot_complete_milestone;

extern alt_freq_t g_ALT_CLK_MPU_frequency;
extern alt_freq_t g_ALT_CLK_DDR_DQS_frequency;
extern alt_freq_t g_ALT_CLK_MPU_PERIPH_frequency;

extern char g_boot_messages_buffer[];
extern char *g_boot_messages_end;

int boot_init(void);
void boot_message(const char *fmt, ...);
void boot_check_status(ALT_STATUS_CODE status, char *file, int line);

#endif /* BOOT_SUPPORT_H_ */
/* md5sum:d8903778cc602f3eead0e1eb3f62117b 2013-09-28 20:48:16 */
