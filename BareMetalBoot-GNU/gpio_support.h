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

#ifndef GPIO_SUPPORT_H_
#define GPIO_SUPPORT_H_

#include "hwlib.h"

#define USER_PB_HPS0	(ALT_HLGPI_8)
#define USER_PB_HPS1	(ALT_HLGPI_9)
#define USER_PB_HPS2	(ALT_HLGPI_10)
#define USER_PB_HPS3	(ALT_HLGPI_11)

#define USER_DIPSW_HPS0	(ALT_HLGPI_4)
#define USER_DIPSW_HPS1	(ALT_HLGPI_5)
#define USER_DIPSW_HPS2	(ALT_HLGPI_6)
#define USER_DIPSW_HPS3	(ALT_HLGPI_7)

#define USER_LED_HPS0	(ALT_GPIO_1BIT_44)
#define USER_LED_HPS1	(ALT_GPIO_1BIT_43)
#define USER_LED_HPS2	(ALT_GPIO_1BIT_42)
#define USER_LED_HPS3	(ALT_GPIO_1BIT_41)

ALT_STATUS_CODE gpio_init(void);
ALT_STATUS_CODE gpio_uninit(void);

#endif /* GPIO_SUPPORT_H_ */
/* md5sum:34668ab5d7f0e9aaf09a15899f8ae70c 2013-09-28 20:48:16 */
