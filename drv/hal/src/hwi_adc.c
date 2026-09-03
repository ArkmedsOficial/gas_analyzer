#include "hwi_adc.h"
#include "bsp_adc.h"

hwi_status_t hwi_adc_init(uint8_t adc_id, const hwi_adc_config_t *config)
{
    if (config == NULL)
        return HWI_ERROR;

    if (adc_id >= HWI_ADC_MAX_ID)
        return HWI_ERROR;

    if (config->resolution >= HWI_ADC_RESOLUTION_COUNT)
        return HWI_ERROR;

    if (config->channel_count == 0 || config->channel_count > HWI_ADC_MAX_CHANNELS)
        return HWI_ERROR;

    for (uint8_t i = 0; i < config->channel_count; i++) {
        if (config->channels[i] >= HWI_ADC_MAX_CHANNELS)
            return HWI_ERROR;
    }

    return bsp_adc_init(adc_id, config);
}

hwi_status_t hwi_adc_deinit(uint8_t adc_id)
{
    if (adc_id >= HWI_ADC_MAX_ID)
        return HWI_ERROR;

    return bsp_adc_deinit(adc_id);
}

hwi_status_t hwi_adc_read_mv(uint8_t adc_id, uint8_t channel, uint16_t *voltage_mv)
{
    int bsp_value_mv = 0;

    if (bsp_adc_read_mv(adc_id, channel, &bsp_value_mv) != HWI_OK)
        return HWI_ERROR;

    *voltage_mv = (uint16_t)bsp_value_mv;
    return HWI_OK;
}
