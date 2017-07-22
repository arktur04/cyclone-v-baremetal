/******************************************************************************
*
* Copyright 2014 Altera Corporation. All Rights Reserved.
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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "alt_interrupt.h"
#include "alt_timers.h"
#include "alt_generalpurpose_io.h"

volatile bool blink = false;

/******************************************************************************/
/*!
 * Timer Module ISR Callback
 *
 * \param       icciar
 * \param       context ISR context.
 * \return      none
 */
static void gp_timer_isr_callback()
{
	// Clear interrupt source don't care about the return value
	alt_gpt_int_clear_pending(ALT_GPT_SP_TMR1);
	// Notify main thread
	// GP_Timer_Interrupt_Fired = true;
	alt_gpio_port_data_write(ALT_GPIO_PORTB, 0x0F000000, blink << 24);
	blink = !blink;
}

/******************************************************************************/
/*!
 * Main entry point
 *
 */
int main(void)
{
	alt_gpio_port_datadir_set(ALT_GPIO_PORTB, 0x0F000000, 0x0F000000);
        // System init
        alt_gpt_all_tmr_init();
        // Setup Interrupt
	alt_int_global_init();
        // Initialize CPU interrupts
        alt_int_cpu_init();
        // Set interrupt distributor target
        int cpu_target = 0x1; //CPU0 will handle the interrupts
        alt_int_dist_target_set(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ, cpu_target);
        // Set interrupt trigger type
        alt_int_dist_trigger_set(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ, ALT_INT_TRIGGER_AUTODETECT);
        // Enable interrupt at the distributor level
        alt_int_dist_enable(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ);
        // Enable CPU interrupts
        alt_int_cpu_enable();
        // Enable global interrupts
        alt_int_global_enable();
        // Set SP Timer 1 to one shot
        alt_gpt_mode_set(ALT_GPT_SP_TMR1, ALT_GPT_RESTART_MODE_PERIODIC);
        // Load SP Timer 1 value
        alt_gpt_counter_set(ALT_GPT_SP_TMR1, 50 * 1000 * 1000); //100 MHz / 50e6 = 2 Hz
        // Register timer ISR
        alt_int_isr_register(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ, gp_timer_isr_callback, NULL);
        // Enable interrupt SP Timer 1
        alt_gpt_int_enable(ALT_GPT_SP_TMR1);
        // Set SP Timer 1 to start running
        alt_gpt_tmr_start(ALT_GPT_SP_TMR1);
	while(1);
	return 0; //unreachable
}

