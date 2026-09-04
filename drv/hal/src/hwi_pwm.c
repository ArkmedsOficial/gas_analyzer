#include "hwi_pwm.h"
#include "bsp_pwm.h"

hwi_status_t hwi_pwm_init(hwi_pwm_t *pwm, const hwi_pwm_config_t *config)
{
    if (pwm == NULL || config == NULL)
        return HWI_ERROR;

    if (config->frequency_hz == 0)
        return HWI_ERROR;

    if (config->duty_percent > 100)
        return HWI_ERROR;

    pwm->port = config->port;
    pwm->pin = config->pin;
    pwm->frequency_hz = config->frequency_hz;
    pwm->duty_percent = config->duty_percent;
    pwm->timer_id = config->timer_id;
    pwm->bsp_handle = NULL;

    return bsp_pwm_init(config, &pwm->bsp_handle);
}

hwi_status_t hwi_pwm_deinit(hwi_pwm_t *pwm)
{
    if (pwm == NULL)
        return HWI_ERROR;

    if (pwm->bsp_handle == NULL)
        return HWI_OK;

    hwi_status_t status = bsp_pwm_deinit(pwm->bsp_handle);
    pwm->bsp_handle = NULL;

    return status;
}

hwi_status_t hwi_pwm_start(hwi_pwm_t *pwm)
{
    if (pwm == NULL || pwm->bsp_handle == NULL)
        return HWI_ERROR;

    return bsp_pwm_start(pwm->bsp_handle);
}

hwi_status_t hwi_pwm_stop(hwi_pwm_t *pwm)
{
    if (pwm == NULL || pwm->bsp_handle == NULL)
        return HWI_ERROR;

    return bsp_pwm_stop(pwm->bsp_handle);
}

hwi_status_t hwi_pwm_set_duty(hwi_pwm_t *pwm, uint8_t duty_percent)
{
    if (pwm == NULL || pwm->bsp_handle == NULL)
        return HWI_ERROR;

    if (duty_percent > 100)
        return HWI_ERROR;

    pwm->duty_percent = duty_percent;

    return bsp_pwm_set_duty(pwm->bsp_handle, duty_percent);
}

hwi_status_t hwi_pwm_set_frequency(hwi_pwm_t *pwm, uint32_t frequency_hz)
{
    if (pwm == NULL || pwm->bsp_handle == NULL)
        return HWI_ERROR;

    if (frequency_hz == 0)
        return HWI_ERROR;

    pwm->frequency_hz = frequency_hz;

    return bsp_pwm_set_frequency(pwm->bsp_handle, frequency_hz);
}
