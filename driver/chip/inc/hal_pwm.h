/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_PWM_H__
#define __HAL_PWM_H__
#include "hal_platform.h"

#ifdef HAL_PWM_MODULE_ENABLED
/**

 * @addtogroup HAL
 * @{
 * @addtogroup PWM
 * @{
 * This section introduces the Pulse-Width Modulation(PWM) APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, PWM function groups, enums, structures and functions.
 * @section HAL_PWM_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b PWM                        | Pulse-Width Modulation. PWM is a modulation technique used to encode a message into a pulsing signal. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Pulse-width_modulation"> PWM in Wikipedia </a>.|
 * @section HAL_PWM_Features_Chapter Supported features
 *
 * - \b Support \b polling \b mode. \n
 *   The PWM generates the fixed waveform at the specified frequency and duty. The application can query current frequency and
 *   duty of the polling mode. The PWM does not support interrupt mode.
 *   \n
 * @section HAL_PWM_Architechture_Chapter Software architecture of PWM
 *
 *
 * -# PWM polling mode architecture.\n
 *   Call hal_pinmux_set_function() please refer to hal_pinmux_define.h. Not need to configure the pinmux, if EPT is used.\n
 *   Call hal_pwm_init() function to initialize the PWM source clock.\n
 *   Call hal_pwm_set_frequency() function to set the PWM frequency.\n
 *   Call hal_pwm_set_duty_cycle() function to set the PWM duty cycle. The duty cycle is calculated as the product of application's duty ratio.\n
 *   Call hal_pwm_start() function to trigger PWM execution.\n
 *   Call hal_pwm_get_frequency() and  hal_pwm_get_duty_cycle() functions if the current frequency and duty cycle need to be retrieved.\n
 *   Call hal_pwm_get_running_status() function to get the PWM status, either busy or idle.\n
 *   Call hal_pwm_stop() function to stop the PWM execution.\n
 *  Polling mode architecture is similar to the polling mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for polling mode architecture.\n
 *
 *
 * @section HAL_PWM_Driver_Usage_Chapter  How to use this driver
 *
 * @}
 * @}
*/
#ifdef HAL_PWM_FEATURE_ADVANCED_CONFIG

/**
 * @addtogroup HAL
 * @{
 * @addtogroup PWM
 * @{
 * - Not support. \n
 *
 *    @endcode
  * @}
 * @}
 */
#else
/**
 * @addtogroup HAL
 * @{
 * @addtogroup PWM
 * @{
 * - Use PWM with polling mode. \n
 *  How to config PWM frequency and duty cycle at fixed souce clock. Assuming the crystal(XTAL) oscillator runs at 26MHz. The applicaton needs a waveform at 2kHz frequency and 50% duty ratio.
 *  The limits of frequency and duty cycle are as shown below.
 *                       1.  The maxinum supported frequency is the half of source clock frequency. When using the 26MHz Xtal as clock source, the maxinum supported frequency is 13MHz.
 *                       2.  PWM duty_cycle, the maxinum duty_cycle is 1000, resolution is 0.1, ex: duty_ratio is 50(represent duty cycle 50%), duty_cycle = (50 / 0.1) = 500.
 *                       3.  total_count = source_clock/frequency, and the value of total_count should greate than 2. It is optional and can be used to assist judgment.
 *                       4.  The frequency and duty cycle should meet the following formula. \n
 *                           T(second)	PWM period \n
 *                           F (Hz)		PWM frequency = 1/T \n
 *                           t (second)  Tick clk period \n
 *                           f (Hz)		Tick clk frequency = 1/t \n
 *                           D (%)		Duty cycle \n
 *                           X (unit t)	Value of configurable register pwm_on_time[15:0], maxinum value is 0xFFFF, in unit t \n
 *                           Y (unit t)	Value of configurable register pwm_off_time[15:0], maxinum value is 0xFFFF, in unit t \n
 *                           Res (step)	PWM resolution of duty cycle on certain F, f \n
 *                           (X + Y) t = T \n
 *                           (X + Y) = T/t = f/F =Res \n
 *                           D = X / (X + Y) \n
 *                           X = D * (X+Y) =  D * f/F \n
 *                           Y = f/F - X = f/F - D * f/F = ((1-D) * f)/F \n
 *                           \b Error \b case: F = 200 Hz ,f = 26MHz, duty_cycle = 255, D = (duty_cycle / 1000) = ((duty_ratio / 0.1) / 1000) \n
 *                           X = D * (X+Y) =  D * f/F  = 0x817E \n
 *                           Y = f/F - X = f/F - D * f/F = ((1 - D) * f)/F = 0x17A52, exceed maxinum value 0xFFFF, will return error \n
 *                           \b Correct \b case: F = 200 Hz ,f = 26MHz, duty_cycle = 500, D = duty_cycle / 1000 = ((duty_ratio / 0.1) / 1000) \n
 *                           X = D * (X+Y) =  D * f/F  = 0xFDE8 \n
 *                           Y = f/F - X = f/F - D * f/F = ((1 - D * )f)/F = 0xFDE8, no exceed maxinum value 0xFFFF \n
 *   .
 *    - Step1: Call hal_pinmux_set_function() to set the pwm pinmux, please refer to hal_pinmux_define.h. Not need to configure the pinmux, if EPT is used.
 *    - Step2: Call hal_pwm_init() to initialize the PWM source clock.
 *    - Step3: Call hal_pwm_set_frequency() to set frequency.
 *    - Step4: Call hal_pwm_set_duty_cycle() to set duty cycle according to application's duty ratio.
 *    - Step5: Call hal_pwm_start() to trigger PWM at a specified frequency and duty count values.
 *    - Step6: Call hal_pwm_get_frequency() to get the current frenquency.
 *    - Step7: Call hal_pwm_get_duty_cycle() to get the current duty_cycle.
 *    - Step8: Call hal_pwm_stop()to stop the PWM.
 *    - Step9: Call hal_pwm_deinit() to deinitialize the PWM.
 *    - Sample code:
 *    @code
 *       uint32_t duty_ratio = 50; //The range from 0 to 100, duty_cycle = duty_ratio * 10.
 *       hal_pwm_source_clock_t source_clock = HAL_PWM_CLOCK_26MHZ;
 *       uint32_t  frequency = 2000;
 *
 *       hal_gpio_init(HAL_GPIO_50);
 *       //function index = MT7933_PIN_50_FUNC_PWM_5; for more information about pinmux please refernence hal_pinmux_define.h
 *       hal_pinmux_set_function(HAL_GPIO_50, function_index);//Set the GPIO pinmux.
 *       //Initialize the PWM source clock.
 *       if(HAL_PWM_STATUS_OK != hal_pwm_init(source_clock)) {
 *             //error handle
 *       }
 *       //Sets frequency and gets total count of the PWM hardware at the specific frequency.
 *       if(HAL_PWM_STATUS_OK != hal_pwm_set_frequency(HAL_PWM_0, frequency, &total_count)) {
 *             //error handle
 *       }
 *       duty_cycle = duty_ratio * 10; //duty_cycle is calcauted as a product of application's duty_ratio.
 *       //Enable PWM to start the timer.
 *       if(HAL_PWM_STATUS_OK != hal_pwm_set_duty_cycle(HAL_PWM_0, duty_cycle)) {
 *             //error handle
 *       }
 *       hal_pwm_start(HAL_PWM_0);  //Trigger PWM execution, start and stop API should appear in pairs.
 *       //...
 *       hal_pwm_get_frequency(HAL_PWM_0, &frequency);
 *       hal_pwm_get_duty_cycle(HAL_PWM_0, &duty_cycle);
 *       //...
 *       hal_pwm_stop(HAL_PWM_0); //Stop the PWM, due to suspend lock, start and stop API should appear in pairs.
 *       hal_pwm_deinit();   //Deinitialize the PWM.
 *
 *    @endcode
  * @}
 * @}
 */

#endif

/**
 * @addtogroup HAL
 * @{
 * @addtogroup PWM
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_pwm_enum Enum
  * @{
  */

/** @brief  This enum defines the API return type.  */
typedef enum {
    HAL_PWM_STATUS_ERROR                = -4,         /**< An error occurred during the function call. */
    HAL_PWM_STATUS_ERROR_CHANNEL        = -3,         /**< A wrong PWM channel is given. */
    HAL_PWM_STATUS_INVALID_PARAMETER    = -2,         /**< A wrong parameter is given. */
    HAL_PWM_STATUS_INVALID_FREQUENCY    = -1,         /**< A smaller frequency is given.*/
    HAL_PWM_STATUS_OK                   =  0          /**< No error during the function call. */
} hal_pwm_status_t;

/** @brief   This enum defines the PWM running status. */
typedef enum {
    HAL_PWM_IDLE = 0,                         /**<The PWM status is idle. */
    HAL_PWM_BUSY = 1                          /**< The PWM status is busy. */
} hal_pwm_running_status_t;

/**
  * @}
  */

#ifdef   HAL_PWM_FEATURE_ADVANCED_CONFIG

/** @defgroup hal_pwm_enum Enum
  * @{
  */

/** @brief  This enum defines PWM clock division advanced configuration */
typedef enum {
    HAL_PWM_CLOCK_DIVISION_2 = 1,            /**< Specify the PWM source clock 2 division. */
    HAL_PWM_CLOCK_DIVISION_4 = 2,            /**< Specify the PWM source clock 4 division. */
    HAL_PWM_CLOCK_DIVISION_8 = 3             /**< Specify the PWM source clock 8 division. */
} hal_pwm_advanced_config_t;

/**
  * @}
  */

#endif

#ifdef HAL_PWM_FEATURE_SINGLE_SOURCE_CLOCK

/**
 * @brief    This function initializes the PWM hardware source clock.
 * @param[in]  source_clock is the PWM source clock, it should be noted that the current driver only supports the clock of HAL_PWM_CLOCK_26MHZ.
 *            For more details about the parameter, please refer to #hal_pwm_source_clock_t.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 * @note    There are some limitations about the minimum frequency that we can set if we choose the related
 *               source clock list in hal_pwm_source_clock_t, and the caculation formula is as follows:\n
 * \b The \b minimum \b frequency \b = \b \b ( \b the \b chosen \b source \b clock\b ) \b / \b (0XFFFF * 2) \n
 * For example:\n
              If we choose the clock of  HAL_PWM_CLOCK_26MHZ, the minimum frequency that we can set is:HAL_PWM_CLOCK_26MHZ /( 0xFFFF * 2) = 199 Hz;
              
 * @warning   For this chip, all the PWM channel is using the same clock once this function has been called.
 *            It should be noted that the current driver only supports the clock of HAL_PWM_CLOCK_26MHZ.
 * @sa  hal_pwm_deinit()
 */

hal_pwm_status_t hal_pwm_init(hal_pwm_source_clock_t source_clock);

/**
 * @brief   This function deinitializes the PWM hardware.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 * @sa  hal_pwm_init()
 */

hal_pwm_status_t hal_pwm_deinit(void);

#else
/**
 * @brief    This function initializes the PWM hardware source clock.
 * @param[in]  pwm_channel is the PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @param[in]  source_clock is the PWM  source clock, it should be noted that the current driver only supports the clock of HAL_PWM_CLOCK_26MHZ.
 *            For more details about the parameter, please refer to #hal_pwm_source_clock_t.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 * @note    There are some limitations about the minimum frequency that we can set if we choose the related
 *               source clock list in hal_pwm_source_clock_t, and the caculation formula is as follows:\n
 * \b The \b minimum \b frequency \b = \b \b ( \b the \b chosen \b source \b clock\b ) \b / \b ( \b 0X1FFF \b * \b advanced_config \b ) \n
 * For example:\n
 *           If we choose the clock of HAL_PWM_CLOCK_26MHZ, the minimum frequency that we can set is 199 Hz;\n
 * @warning  For this chip, we can config the PWM channel and the related clock, which means different PWM channels can use the different clock,
 *           But the current driver only supports the clock of HAL_PWM_CLOCK_26MHZ.
 * @sa  hal_pwm_deinit()
 */

hal_pwm_status_t hal_pwm_init(hal_pwm_channel_t pwm_channel, hal_pwm_source_clock_t source_clock);

/**
 * @brief   This function deinitializes the PWM hardware.
 * @param[in]  pwm_channel is the PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 * @sa  hal_pwm_init()
 */
hal_pwm_status_t hal_pwm_deinit(hal_pwm_channel_t pwm_channel);

#endif

/**
 * @brief This function sets the PWM frequency and retrieves total count of the PWM hardware at the specified frequency.
 * @param[in]  pwm_channel is the PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @param[in]  frequency is the PWM output frequency, the maxinum supported frequency is the half of source clock frequency.
 * @param[out]   total_count is PWM hardware total count, the value of this parameter varies based on the given PWM frequency.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 */
hal_pwm_status_t hal_pwm_set_frequency(hal_pwm_channel_t pwm_channel, uint32_t frequency, uint32_t *total_count);

/**
 * @brief This function sets the PWM  duty cycle.
 * @param[in]  pwm_channel is the PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @param[in]  duty_cycle is the PWM hardware duty cycle, PWM duty_cycle, the maxinum duty_cycle is 1000, which is calculated as a product of application's duty ratio and hardware 's total count.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 * @sa  hal_pwm_get_duty_cycle()
 */

hal_pwm_status_t hal_pwm_set_duty_cycle(hal_pwm_channel_t pwm_channel, uint32_t duty_cycle);

/**
 * @brief This function starts the PWM execution.
 * @param[in]  pwm_channel is the PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 * @sa  hal_pwm_stop()
 */

hal_pwm_status_t hal_pwm_start(hal_pwm_channel_t pwm_channel);

/**
 * @brief  This function stops the PWM execution.
 * @param[in]  pwm_channel is PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 * @sa  hal_pwm_start()
 */

hal_pwm_status_t hal_pwm_stop(hal_pwm_channel_t pwm_channel);

/**
 * @brief This function gets current frequency of the PWM, the unit of frequency is Hz.
 * @param[in]  pwm_channel is the PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @param[out]  frequency  is PWM output frequency.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 * @sa  hal_pwm_set_frequency()
 */

hal_pwm_status_t hal_pwm_get_frequency(hal_pwm_channel_t pwm_channel, uint32_t *frequency);

/**
 * @brief  This function gets the current duty cycle of the PWM.
 * @param[in]  pwm_channel is the PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @param[out]   *duty_cycle is PWM hardware duty cycle, which is calculated as a product of application's duty ratio and hardware 's total count.
 *            Due to the accuracy of HW, there is a deviation between the actual value and the preset value.
 *            For example: duty_cycle = 0xFF = 255, *duty_cycle will be about 0xFE.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 * @sa  hal_pwm_set_duty_cycle()
 */

hal_pwm_status_t hal_pwm_get_duty_cycle(hal_pwm_channel_t pwm_channel, uint32_t *duty_cycle);

/**
 * @brief  This function gets the current status of PWM.
 * @param[in]  pwm_channel is the PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @param[out]  running_status is PWM busy or idle status, For details about this parameter, please refer to #hal_pwm_running_status_t.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 */

hal_pwm_status_t hal_pwm_get_running_status(hal_pwm_channel_t pwm_channel, hal_pwm_running_status_t *running_status);

#ifdef  HAL_PWM_FEATURE_ADVANCED_CONFIG

/**
 * @brief  This function sets the PWM advanced configuration.
 * @param[in]  pwm_channel is the PWM channel number. For more details about the parameter, please refer to #hal_pwm_channel_t.
 * @param[in]   advanced_config is  PWM source clock division value, For more details about this parameter, please refer to #hal_pwm_advanced_config_t.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #HAL_PWM_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_PWM_STATUS_INVALID_PARAMETER, a wrong parameter is given. The parameter needs to be verified.
 * @par       Example
 * For a sample code, please refer to @ref HAL_PWM_Driver_Usage_Chapter.
 */

hal_pwm_status_t hal_pwm_set_advanced_config(hal_pwm_channel_t pwm_channel, hal_pwm_advanced_config_t advanced_config);

#else

#endif


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_PWM_MODULE_ENABLED*/
#endif /* __HAL_PWM_H__ */

