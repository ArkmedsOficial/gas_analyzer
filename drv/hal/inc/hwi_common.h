#ifndef HWI_COMMON_H
#define HWI_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* bsp_config.h carries board/MCU-specific limits (max peripheral IDs,
 * channel counts, clock ceilings, etc.) and is provided by the concrete BSP
 * once a target MCU is chosen. Until then, each hwi_*.h peripheral header
 * falls back to its own conservative defaults, so the HAL and everything
 * built on top of it can be implemented and unit-tested without depending
 * on any chip-specific file. */
#if defined(__has_include)
    #if __has_include("bsp_config.h")
        #include "bsp_config.h"
    #endif
#endif

typedef enum {
    HWI_OK = 0,
    HWI_ERROR = 1,
} hwi_status_t;

#endif /* HWI_COMMON_H */
