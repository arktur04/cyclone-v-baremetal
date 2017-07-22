#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "alt_interrupt.h"
#include "alt_timers.h"
#include "alt_generalpurpose_io.h"
#include "alt_clock_manager.h"

volatile bool blink = false;

/******************************************************************************/
/*!
 * ISR Callback
 *
 * \param       icciar
 * \param       context ISR context.
 * \return      none
 */
static void gpio_isr_callback() {
	// Clear interrupt source don't care about the return value
	alt_gpio_port_int_status_clear(ALT_GPIO_PORTC, 0x00200000);
	alt_gpio_port_data_write(ALT_GPIO_PORTB, 0x0F000000, blink << 24);
	blink = !blink;
}

/******************************************************************************/
/*!
 * Main entry point
 *
 */
int main(void) {
	alt_gpio_init();
	alt_gpio_port_datadir_set(ALT_GPIO_PORTB, 0x0F000000, 0x0F000000);
        // System init
        alt_gpt_all_tmr_init();
        // Setup Interrupt
	alt_int_global_init();
        // Initialize CPU interrupts
        alt_int_cpu_init();
        // Set interrupt distributor target
        int cpu_target = 0x1; //CPU0 will handle the interrupts
        alt_int_dist_target_set(ALT_INT_INTERRUPT_GPIO2, cpu_target);
        // Set interrupt trigger type
        alt_int_dist_trigger_set(ALT_INT_INTERRUPT_GPIO2, ALT_INT_TRIGGER_AUTODETECT);
        // Enable interrupt at the distributor level
        alt_int_dist_enable(ALT_INT_INTERRUPT_GPIO2);
        // Enable CPU interrupts
        alt_int_cpu_enable();
        // Enable global interrupts
        alt_int_global_enable();
	//debounce clock
	alt_clk_divider_set(ALT_CLK_GPIO_DB, 1 << 17);
	//alt_clk_clock_enable(ALT_CLK_GPIO_DB);

        alt_gpio_port_datadir_set(ALT_GPIO_PORTC, 0x01e00000, 0x00000000);
	alt_gpio_port_debounce_set(ALT_GPIO_PORTC, 0x01e00000, 0x00200000);
	alt_gpio_port_int_type_set(ALT_GPIO_PORTC, 0x01e00000, 0x00200000);
	alt_gpio_port_int_pol_set(ALT_GPIO_PORTC, 0x01e00000, 0x00200000);

	// Register gpio ISR
	alt_gpio_port_int_enable(ALT_GPIO_PORTC, 0x00200000);
	alt_int_isr_register(ALT_INT_INTERRUPT_GPIO2, gpio_isr_callback, NULL);

	while(1);
	return 0; //unreachable
}

