/**
 * \file
 *
 * \brief SAM I2S - Inter-IC Sound Controller Driver Quick Start
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \page asfdoc_sam0_i2s_callback_use_case Quick Start Guide for I2S - Callback
 *
 * In this use case, the I<SUP>2</SUP>S will be used to generate Master Clock (MCK), Serial
 * Clock (SCK), Frame Sync (FS), and Serial Data (SD) signals.
 *
 * Here MCK is set to the half of processor clock. SCK is set to a quarter of
 * the frequency of processor. FS generates half-half square wave for left and
 * right audio channel data. The output serial data of channels toggle from two
 * values to generate square wave, if codec or DAC is connected.
 *
 * The I<SUP>2</SUP>S module will be set up as follows:
 *
 * - GCLK generator 0 (GCLK main) clock source
 * - MCK, SCK, and FS clocks outputs are enabled
 * - MCK output divider set to 2
 * - SCK generation divider set to 4
 * - Each frame will contain two 32-bit slots
 * - Data will be left adjusted and start transmit without delay
 *
 * \section asfdoc_sam0_i2s_callback_use_case_setup Quick Start
 *
 * \subsection asfdoc_sam0_i2s_callback_use_case_prereq Prerequisites
 * There are no prerequisites for this use case.
 *
 * \subsection asfdoc_sam0_i2s_callback_use_case_setup_code Code
 *
 * Add to the main application source file, before any functions:
 * \snippet conf_i2s_quick_start_callback.h definition_i2s
 *
 * Add to the main application source file, outside of any functions:
 * \snippet qs_i2s_callback.c module_inst
 *
 * Copy-paste the following data buffer code to your user application:
 * \snippet qs_i2s_callback.c data_buffer
 *
 * Copy-paste the following callback function code to your user application:
 * \snippet qs_i2s_callback.c callback_funcs
 *
 * Copy-paste the following setup code to your user application:
 * \snippet qs_i2s_callback.c setup
 *
 * Add to user application initialization (typically the start of \c main()):
 * \snippet qs_i2s_callback.c setup_init
 *
 * Add to user application start transmitting job (typically in \c main(), after
 * initialization):
 * \snippet qs_i2s_callback.c start_job
 *
 * \subsection asfdoc_sam0_i2s_callback_use_case_setup_flow Workflow
 * -# Create a module software instance structure for the I<SUP>2</SUP>S module to store
 *    the I<SUP>2</SUP>S driver state while it is in use.
 *    \snippet qs_i2s_callback.c module_inst
 *    \note This should never go out of scope as long as the module is in use.
 *          In most cases, this should be global.
 *
 * -# Configure the I<SUP>2</SUP>S module.
 *  -# Initialize the I<SUP>2</SUP>S module.
 *     \snippet qs_i2s_callback.c setup_i2s_init
 *  -# Initialize the I<SUP>2</SUP>S Clock Unit.
 *   -# Create a I<SUP>2</SUP>S module configuration struct, which can be filled out to
 *      adjust the configuration of a physical I<SUP>2</SUP>S Clock Unit.
 *      \snippet qs_i2s_callback.c setup_clock_unit_config
 *   -# Initialize the I<SUP>2</SUP>S Clock Unit configuration struct with the module's
 *      default values.
 *      \snippet qs_i2s_callback.c setup_clock_unit_config_defaults
 *      \note This should always be performed before using the configuration
 *            struct to ensure that all values are initialized to known default
 *            settings.
 *
 *   -# Alter the I<SUP>2</SUP>S Clock Unit settings to configure the general clock source,
 *      MCK, SCK, and FS generation.
 *      \snippet qs_i2s_callback.c setup_clock_unit_change_config
 *   -# Alter the I<SUP>2</SUP>S Clock Unit settings to configure the MCK, SCK, and FS
 *      output on physical device pins.
 *      \snippet qs_i2s_callback.c setup_clock_unit_change_pins
 *   -# Configure the I<SUP>2</SUP>S Clock Unit with the desired settings.
 *      \snippet qs_i2s_callback.c setup_clock_unit_set_config
 *  -# Initialize the I<SUP>2</SUP>S Serializer.
 *   -# Create a I<SUP>2</SUP>S Serializer configuration struct, which can be filled out to
 *      adjust the configuration of a physical I<SUP>2</SUP>S Serializer.
 *      \snippet qs_i2s_callback.c setup_serializer_config
 *   -# Initialize the I<SUP>2</SUP>S Serializer configuration struct with the module's
 *      default values.
 *      \snippet qs_i2s_callback.c setup_serializer_config_defaults
 *      \note This should always be performed before using the configuration
 *            struct to ensure that all values are initialized to known default
 *            settings.
 *
 *   -# Alter the I<SUP>2</SUP>S Serializer settings to configure the serial
 *      data generation.
 *      \snippet qs_i2s_callback.c setup_serializer_change_config_tx
 *   -# Alter the I<SUP>2</SUP>S Serializer settings to configure the SD on a physical
 *      device pin.
 *      \snippet qs_i2s_basic.c setup_serializer_change_config_pin_tx
 *   -# Configure the I<SUP>2</SUP>S Serializer with the desired settings.
 *      \snippet qs_i2s_callback.c setup_serializer_set_config_tx
 *  -# Enable the I<SUP>2</SUP>S module, the Clock Unit, and Serializer to start the clocks
 *     and ready to transmit data.
 *     \snippet qs_i2s_callback.c setup_enable
 * -# Configure the I<SUP>2</SUP>S callbacks.
 *  -# Register the Serializer 0 TX ready callback function with the driver.
 *     \snippet qs_i2s_callback.c setup_register_callback
 *  -# Enable the Serializer 0 TX ready callback so that it will be called by
 *     the driver when appropriate.
 *     \snippet qs_i2s_callback.c setup_enable_callback
 * -# Start a transmitting job.
 *    \snippet qs_i2s_callback.c start_job
 *
 * \section asfdoc_sam0_i2s_callback_use_case_main Use Case
 *
 * \subsection asfdoc_sam0_i2s_callback_use_case_main_code Code
 * Copy-paste the following code to your user application:
 * \snippet qs_i2s_callback.c main
 *
 * \subsection asfdoc_sam0_i2s_callback_use_case_main_flow Workflow
 * -# Enter an infinite loop while the output is generated via the I<SUP>2</SUP>S module.
 *    \snippet qs_i2s_callback.c main_loop
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
