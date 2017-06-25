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

#include "alt_timers.h"
#include "alt_interrupt.h"
#include "pit_support.h"
#include "boot_support.h"

volatile uint32_t g_PIT_count = 0;
volatile uint32_t g_PIT_clear_error = ALT_E_SUCCESS;

static void PIT_IRQ_handler(uint32_t icciar, void * context) {

	ALT_STATUS_CODE status;

    *((volatile uint32_t*)(context)) += 1;

	status = alt_gpt_int_clear_pending(ALT_GPT_CPU_PRIVATE_TMR);
	if(status != ALT_E_SUCCESS)
		g_PIT_clear_error = status;

	return;
}

ALT_STATUS_CODE pit_init(void) {

	ALT_STATUS_CODE status = ALT_E_SUCCESS;
	alt_freq_t temp_frequency;

	status = alt_gpt_all_tmr_init();
	BOOT_CHECK_STATUS;

	status = alt_clk_freq_get(ALT_CLK_MPU_PERIPH, &temp_frequency);
	BOOT_CHECK_STATUS;

	status = alt_gpt_counter_set(ALT_GPT_CPU_PRIVATE_TMR, temp_frequency / 100);
	BOOT_CHECK_STATUS;

	if(alt_gpt_time_microsecs_get(ALT_GPT_CPU_PRIVATE_TMR) != 10000)
		status = ALT_E_ERROR;
	BOOT_CHECK_STATUS;

	status = alt_gpt_mode_set(ALT_GPT_CPU_PRIVATE_TMR, ALT_GPT_RESTART_MODE_PERIODIC);
	BOOT_CHECK_STATUS;

	status = alt_gpt_tmr_start(ALT_GPT_CPU_PRIVATE_TMR);
	BOOT_CHECK_STATUS;

	return status;
}

ALT_STATUS_CODE pit_interrupt_enable(void) {

	ALT_STATUS_CODE status = ALT_E_SUCCESS;

	g_PIT_count = 0;

	status = alt_int_isr_register(ALT_INT_INTERRUPT_PPI_TIMER_PRIVATE, PIT_IRQ_handler, (void *)(&g_PIT_count));
	BOOT_CHECK_STATUS;

    status = alt_int_dist_enable(ALT_INT_INTERRUPT_PPI_TIMER_PRIVATE);
	BOOT_CHECK_STATUS;

	status = alt_gpt_int_clear_pending(ALT_GPT_CPU_PRIVATE_TMR);
	BOOT_CHECK_STATUS;

	status = alt_gpt_int_enable(ALT_GPT_CPU_PRIVATE_TMR);
	BOOT_CHECK_STATUS;

	return status;
}
/* md5sum:b4d5b9fac130a06f876d97d14ae0be3a 2013-09-28 20:48:16 */
