#ifndef HWI_ADC_H
#define HWI_ADC_H

#include "hwi_common.h"

#ifndef BSP_ADC_MAX_ID
    #define HWI_ADC_MAX_ID  2
#else
    #define HWI_ADC_MAX_ID  BSP_ADC_MAX_ID
#endif

#ifndef BSP_ADC_MAX_CHANNELS
    #define HWI_ADC_MAX_CHANNELS  4
#else
    #define HWI_ADC_MAX_CHANNELS  BSP_ADC_MAX_CHANNELS
#endif

typedef enum {
    HWI_ADC_RESOLUTION_8BIT = 0,
    HWI_ADC_RESOLUTION_9BIT,
    HWI_ADC_RESOLUTION_10BIT,
    HWI_ADC_RESOLUTION_12BIT,
    HWI_ADC_RESOLUTION_COUNT
} hwi_adc_resolution_t;

typedef struct {
    hwi_adc_resolution_t resolution;
    uint8_t channels[HWI_ADC_MAX_CHANNELS];
    uint8_t channel_count;
} hwi_adc_config_t;

hwi_status_t hwi_adc_init(uint8_t adc_id, const hwi_adc_config_t *config);
hwi_status_t hwi_adc_deinit(uint8_t adc_id);
hwi_status_t hwi_adc_read_mv(uint8_t adc_id, uint8_t channel, uint16_t *voltage_mv);

#endif /* HWI_ADC_H */
