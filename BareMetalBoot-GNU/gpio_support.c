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

#include "alt_generalpurpose_io.h"
#include "gpio_support.h"
#include "boot_support.h"

ALT_STATUS_CODE gpio_init(void) {

	//
	// Populate the GPIO configuration array
	//
	ALT_GPIO_CONFIG_RECORD_t config_array[] =
	{
			{
				.signal_number = USER_PB_HPS0,
				.direction = ALT_GPIO_PIN_INPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_DEBOUNCE,
				.data = ALT_GPIO_PIN_DATAZERO
			},

			{
				.signal_number = USER_PB_HPS1,
				.direction = ALT_GPIO_PIN_INPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_DEBOUNCE,
				.data = ALT_GPIO_PIN_DATAZERO
			},

			{
				.signal_number = USER_PB_HPS2,
				.direction = ALT_GPIO_PIN_INPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_DEBOUNCE,
				.data = ALT_GPIO_PIN_DATAZERO
			},

			{
				.signal_number = USER_PB_HPS3,
				.direction = ALT_GPIO_PIN_INPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_DEBOUNCE,
				.data = ALT_GPIO_PIN_DATAZERO
			},

			{
				.signal_number = USER_DIPSW_HPS0,
				.direction = ALT_GPIO_PIN_INPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_NODEBOUNCE,
				.data = ALT_GPIO_PIN_DATAZERO
			},

			{
				.signal_number = USER_DIPSW_HPS1,
				.direction = ALT_GPIO_PIN_INPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_NODEBOUNCE,
				.data = ALT_GPIO_PIN_DATAZERO
			},

			{
				.signal_number = USER_DIPSW_HPS2,
				.direction = ALT_GPIO_PIN_INPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_NODEBOUNCE,
				.data = ALT_GPIO_PIN_DATAZERO
			},

			{
				.signal_number = USER_DIPSW_HPS3,
				.direction = ALT_GPIO_PIN_INPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_NODEBOUNCE,
				.data = ALT_GPIO_PIN_DATAZERO
			},

			{
				.signal_number = USER_LED_HPS0,
				.direction = ALT_GPIO_PIN_OUTPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_NODEBOUNCE,
				.data = ALT_GPIO_PIN_DATAONE
			},

			{
				.signal_number = USER_LED_HPS1,
				.direction = ALT_GPIO_PIN_OUTPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_NODEBOUNCE,
				.data = ALT_GPIO_PIN_DATAONE
			},

			{
				.signal_number = USER_LED_HPS2,
				.direction = ALT_GPIO_PIN_OUTPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_NODEBOUNCE,
				.data = ALT_GPIO_PIN_DATAONE
			},

			{
				.signal_number = USER_LED_HPS3,
				.direction = ALT_GPIO_PIN_OUTPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_NODEBOUNCE,
				.data = ALT_GPIO_PIN_DATAONE
			},

			{
				.signal_number = ALT_END_OF_GPIO_SIGNALS,
				.direction = ALT_GPIO_PIN_INPUT,
				.type = ALT_GPIO_PIN_LEVEL_TRIG_INT,
				.polarity = ALT_GPIO_PIN_ACTIVE_LOW,
				.debounce = ALT_GPIO_PIN_NODEBOUNCE,
				.data = ALT_GPIO_PIN_DATAZERO
			}
	};

	ALT_STATUS_CODE status = ALT_E_SUCCESS;

	status = alt_gpio_init();
	BOOT_CHECK_STATUS;

	status = alt_gpio_group_config(config_array, ARRAY_COUNT(config_array));
	BOOT_CHECK_STATUS;

	return status;
}

ALT_STATUS_CODE gpio_uninit(void) {

	return alt_gpio_init();
}
/* md5sum:d88b202fc968fca643011ed4debad737 2013-09-28 20:48:16 */
