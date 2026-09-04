#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "hwi_adc.h"

/* Contract the concrete BSP (board/MCU-specific) must fulfill for hwi_adc.
 * No implementation lives here: this header only exists so hwi_adc can be
 * built and unit-tested (via a CMock-generated mock_bsp_adc) before a
 * target MCU is chosen. The real drv/bsp_<mcu>/src/bsp_adc.c implementing
 * these functions comes later, once hardware is selected. */
hwi_status_t bsp_adc_init(uint8_t adc_id, const hwi_adc_config_t *config);
hwi_status_t bsp_adc_deinit(uint8_t adc_id);
hwi_status_t bsp_adc_read_mv(uint8_t adc_id, uint8_t channel, int *out_mv);

#endif /* BSP_ADC_H */
