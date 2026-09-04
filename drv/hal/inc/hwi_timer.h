#ifndef HWI_TIMER_H
#define HWI_TIMER_H

#include "hwi_common.h"

#ifndef BSP_TIMER_MAX_ID
    #define HWI_TIMER_MAX_ID  4
#else
    #define HWI_TIMER_MAX_ID  BSP_TIMER_MAX_ID
#endif

#ifndef BSP_TIMER_MAX_FREQUENCY_HZ
    #define HWI_TIMER_MAX_FREQUENCY_HZ  10000000
#else
    #define HWI_TIMER_MAX_FREQUENCY_HZ  BSP_TIMER_MAX_FREQUENCY_HZ
#endif

typedef enum {
    HWI_TIMER_MODE_PERIODIC = 0,
    HWI_TIMER_MODE_ONE_SHOT,
    HWI_TIMER_MODE_COUNT
} hwi_timer_mode_t;

typedef struct {
    uint32_t frequency_hz;
    hwi_timer_mode_t mode;
} hwi_timer_config_t;

typedef void (*hwi_timer_callback_t)(void);

hwi_status_t hwi_timer_init(uint8_t timer_id, const hwi_timer_config_t *config);
hwi_status_t hwi_timer_deinit(uint8_t timer_id);
hwi_status_t hwi_timer_start(uint8_t timer_id);
hwi_status_t hwi_timer_stop(uint8_t timer_id);
hwi_status_t hwi_timer_register_callback(uint8_t timer_id, hwi_timer_callback_t callback);
void hwi_timer_isr(uint8_t timer_id);

#endif /* HWI_TIMER_H */
