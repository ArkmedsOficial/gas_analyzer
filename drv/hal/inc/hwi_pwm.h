#ifndef HWI_PWM_H
#define HWI_PWM_H

#include "hwi_common.h"

typedef struct {
    uint8_t port;           /* logical port (MCU-specific; may be unused) */
    uint8_t pin;             /* physical pin number */
    uint32_t frequency_hz;   /* desired PWM frequency in Hz */
    uint8_t duty_percent;    /* duty cycle in percent (0-100) */
    uint8_t timer_id;        /* optional timer/channel id (MCU-specific) */
} hwi_pwm_config_t;

typedef struct {
    uint8_t port;
    uint8_t pin;
    uint32_t frequency_hz;
    uint8_t duty_percent;
    uint8_t timer_id;
    void *bsp_handle; /* opaque BSP-specific handle */
} hwi_pwm_t;

hwi_status_t hwi_pwm_init(hwi_pwm_t *pwm, const hwi_pwm_config_t *config);
hwi_status_t hwi_pwm_deinit(hwi_pwm_t *pwm);
hwi_status_t hwi_pwm_start(hwi_pwm_t *pwm);
hwi_status_t hwi_pwm_stop(hwi_pwm_t *pwm);
hwi_status_t hwi_pwm_set_duty(hwi_pwm_t *pwm, uint8_t duty_percent);
hwi_status_t hwi_pwm_set_frequency(hwi_pwm_t *pwm, uint32_t frequency_hz);

#endif /* HWI_PWM_H */
