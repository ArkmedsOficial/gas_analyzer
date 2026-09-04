#include "qgc_sensor.h"

static uint8_t channel_count_for_type(qgc_sensor_type_t type)
{
    switch (type) {
        case QGC_SENSOR_TYPE_DUAL:
            return 2;
        case QGC_SENSOR_TYPE_QUAD:
            return 4;
        default:
            return 0;
    }
}

hwi_status_t qgc_sensor_init(qgc_sensor_t *sensor, const qgc_sensor_config_t *config)
{
    if (sensor == NULL || config == NULL)
        return HWI_ERROR;

    uint8_t channel_count = channel_count_for_type(config->type);
    if (channel_count == 0)
        return HWI_ERROR;

    hwi_adc_config_t adc_config = {0};
    adc_config.resolution = config->resolution;
    adc_config.channel_count = channel_count;
    for (uint8_t i = 0; i < channel_count; i++)
        adc_config.channels[i] = config->adc_channels[i];

    if (hwi_adc_init(config->adc_id, &adc_config) != HWI_OK)
        return HWI_ERROR;

    sensor->adc_id = config->adc_id;
    sensor->channel_count = channel_count;
    for (uint8_t i = 0; i < channel_count; i++)
        sensor->adc_channels[i] = config->adc_channels[i];

    return HWI_OK;
}

uint8_t qgc_sensor_channel_count(const qgc_sensor_t *sensor)
{
    if (sensor == NULL)
        return 0;

    return sensor->channel_count;
}

hwi_status_t qgc_sensor_read_channel_mv(const qgc_sensor_t *sensor, uint8_t channel_index, uint16_t *voltage_mv)
{
    if (sensor == NULL || voltage_mv == NULL)
        return HWI_ERROR;

    if (channel_index >= sensor->channel_count)
        return HWI_ERROR;

    return hwi_adc_read_mv(sensor->adc_id, sensor->adc_channels[channel_index], voltage_mv);
}
