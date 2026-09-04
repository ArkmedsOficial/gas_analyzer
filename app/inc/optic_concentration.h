#ifndef OPTIC_CONCENTRATION_H
#define OPTIC_CONCENTRATION_H

#include <stdint.h>

#include "optic_channel.h"

/* Beer-Lambert absorption model for a single channel/identified-agent
 * pairing. Depends on the physical characterization of the sensor and
 * agent, and must remain configurable rather than hardcoded. */
typedef struct {
    /* Combined optical absorption constant (extinction coefficient x path
     * length), expressed as absorbance per percent of concentration and
     * scaled by 1000. */
    uint32_t absorption_constant_x1000;
    /* Maximum physically valid concentration for this chamber/agent
     * pairing, as a percentage scaled by 1000. A calculated concentration
     * above it is reported as OPTIC_ERROR_OUT_OF_RANGE. A value of 0
     * disables the check. */
    uint32_t max_valid_concentration_pct_x1000;
} optic_concentration_config_t;

typedef struct {
    /* Gas concentration, as a percentage scaled by 1000 (e.g. 20000 = 20%). */
    uint32_t concentration_pct_x1000;
} optic_concentration_result_t;

optic_status_t optic_concentration_calculate(const optic_concentration_config_t *config,
                                              uint16_t signal_ratio_x1000,
                                              optic_concentration_result_t *out);

#endif /* OPTIC_CONCENTRATION_H */
