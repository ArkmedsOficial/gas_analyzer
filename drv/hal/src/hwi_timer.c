#include "hwi_timer.h"
#include "bsp_timer.h"

static hwi_timer_callback_t registered_callbacks[HWI_TIMER_MAX_ID] = {NULL};

hwi_status_t hwi_timer_init(uint8_t timer_id, const hwi_timer_config_t *config)
{
    if (config == NULL)
        return HWI_ERROR;

    if (config->frequency_hz == 0 || config->frequency_hz > HWI_TIMER_MAX_FREQUENCY_HZ)
        return HWI_ERROR;

    if (config->mode >= HWI_TIMER_MODE_COUNT)
        return HWI_ERROR;

    if (timer_id >= HWI_TIMER_MAX_ID)
        return HWI_ERROR;

    return bsp_timer_init(timer_id, config);
}

hwi_status_t hwi_timer_deinit(uint8_t timer_id)
{
    if (timer_id >= HWI_TIMER_MAX_ID)
        return HWI_ERROR;

    registered_callbacks[timer_id] = NULL;

    return bsp_timer_deinit(timer_id);
}

hwi_status_t hwi_timer_start(uint8_t timer_id)
{
    if (timer_id >= HWI_TIMER_MAX_ID)
        return HWI_ERROR;

    return bsp_timer_start(timer_id);
}

hwi_status_t hwi_timer_stop(uint8_t timer_id)
{
    if (timer_id >= HWI_TIMER_MAX_ID)
        return HWI_ERROR;

    return bsp_timer_stop(timer_id);
}

hwi_status_t hwi_timer_register_callback(uint8_t timer_id, hwi_timer_callback_t callback)
{
    if (callback == NULL)
        return HWI_ERROR;

    if (timer_id >= HWI_TIMER_MAX_ID)
        return HWI_ERROR;

    registered_callbacks[timer_id] = callback;

    return HWI_OK;
}

void hwi_timer_isr(uint8_t timer_id)
{
    if (timer_id >= HWI_TIMER_MAX_ID)
        return;

    if (registered_callbacks[timer_id] != NULL)
        registered_callbacks[timer_id]();
}
