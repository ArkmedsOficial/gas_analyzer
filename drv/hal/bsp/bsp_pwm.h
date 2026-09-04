#ifndef BSP_PWM_H
#define BSP_PWM_H

#include "hwi_pwm.h"

/* Contract the concrete BSP must fulfill for hwi_pwm. As with bsp_adc.h and
 * bsp_timer.h, only the interface exists here; the real
 * drv/bsp_<mcu>/src/bsp_pwm.c comes once a target MCU is chosen. The opaque
 * handle lets each BSP represent the underlying peripheral however it
 * needs to (e.g. an ESP32 MCPWM operator/comparator/generator group)
 * without hwi_pwm knowing anything about it. */
hwi_status_t bsp_pwm_init(const hwi_pwm_config_t *config, void **out_handle);
hwi_status_t bsp_pwm_deinit(void *handle);
hwi_status_t bsp_pwm_start(void *handle);
hwi_status_t bsp_pwm_stop(void *handle);
hwi_status_t bsp_pwm_set_duty(void *handle, uint32_t duty_percent);
hwi_status_t bsp_pwm_set_frequency(void *handle, uint32_t frequency_hz);

#endif /* BSP_PWM_H */
