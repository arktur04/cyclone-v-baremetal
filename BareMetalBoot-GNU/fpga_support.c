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

#include "alt_fpga_manager.h"
#include "fpga_support.h"
#include "boot_support.h"

#if 0
ALT_STATUS_CODE fpga_init(void) {

	ALT_STATUS_CODE status = ALT_E_SUCCESS;
	ALT_FPGA_STATE_t fpga_state;

	status = alt_fpga_init();
	BOOT_CHECK_STATUS;

	status = alt_fpga_control_enable();
	BOOT_CHECK_STATUS;

	status = alt_fgpa_reset_assert();
	BOOT_CHECK_STATUS;

	status = alt_fgpa_reset_deassert();
	BOOT_CHECK_STATUS;

	status = alt_fgpa_reset_deassert();
	BOOT_CHECK_STATUS;

    const char *   fpga_image      = &_binary_soc_system_rbf_start;
    const uint32_t fpga_image_size = &_binary_soc_system_rbf_end - &_binary_soc_system_rbf_start;

	status = alt_fpga_configure(fpga_image, fpga_image_size);
	BOOT_CHECK_STATUS;

	fpga_state = alt_fpga_state_get();
	if(fpga_state != ALT_FPGA_STATE_USER_MODE)
		status = ALT_E_ERROR;
	BOOT_CHECK_STATUS;

	return status;
}
#endif
/* md5sum:e268b2157060033c39e9ea2c5068165f 2013-09-28 20:48:16 */
