#ifndef QGC_SENSOR_H
#define QGC_SENSOR_H

#include "hwi_adc.h"
#include "hwi_common.h"

/* KEMET QGC pyroelectric infrared gas sensor family: purely analog output
 * (no digital/I2C protocol), so "driving" the sensor is really just
 * reading the right ADC channels -- there is no register handshake. */
typedef enum {
    QGC_SENSOR_TYPE_DUAL = 0, /* USEQGCDAANA100, 2 channels */
    QGC_SENSOR_TYPE_QUAD      /* USEQGCQAAN2100, 4 channels */
} qgc_sensor_type_t;

typedef struct {
    qgc_sensor_type_t type;
    uint8_t adc_id;
    /* Which physical ADC channel each sensor channel is wired to, indexed
     * 0..(channel count for `type` - 1). Board wiring, so configurable
     * rather than hardcoded. */
    uint8_t adc_channels[HWI_ADC_MAX_CHANNELS];
    hwi_adc_resolution_t resolution;
} qgc_sensor_config_t;

typedef struct {
    uint8_t adc_id;
    uint8_t channel_count;
    uint8_t adc_channels[HWI_ADC_MAX_CHANNELS];
} qgc_sensor_t;

hwi_status_t qgc_sensor_init(qgc_sensor_t *sensor, const qgc_sensor_config_t *config);
uint8_t qgc_sensor_channel_count(const qgc_sensor_t *sensor);
hwi_status_t qgc_sensor_read_channel_mv(const qgc_sensor_t *sensor, uint8_t channel_index, uint16_t *voltage_mv);

#endif /* QGC_SENSOR_H */
