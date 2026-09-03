#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include "hwi_timer.h"

/* Contract the concrete BSP must fulfill for hwi_timer. As with bsp_adc.h,
 * only the interface exists here; the real drv/bsp_<mcu>/src/bsp_timer.c
 * comes once a target MCU is chosen. hwi_timer_isr (not part of this
 * contract) is what the BSP's real interrupt handler is expected to call
 * once that implementation exists. */
hwi_status_t bsp_timer_init(uint8_t timer_id, const hwi_timer_config_t *config);
hwi_status_t bsp_timer_deinit(uint8_t timer_id);
hwi_status_t bsp_timer_start(uint8_t timer_id);
hwi_status_t bsp_timer_stop(uint8_t timer_id);

#endif /* BSP_TIMER_H */
