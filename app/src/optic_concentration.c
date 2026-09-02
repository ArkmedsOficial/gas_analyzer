#include <math.h>
#include <stddef.h>

#include "optic_concentration.h"

optic_status_t optic_concentration_calculate(const optic_concentration_config_t *config,
                                              uint16_t signal_ratio_x1000,
                                              optic_concentration_result_t *out)
{
    if (config == NULL || out == NULL)
        return OPTIC_ERROR_NULL_POINTER;

    if (config->absorption_constant_x1000 == 0)
        return OPTIC_ERROR_INVALID_PARAM;

    if (signal_ratio_x1000 == 0 || signal_ratio_x1000 > 1000)
        return OPTIC_ERROR_INVALID_PARAM;

    double transmittance = (double)signal_ratio_x1000 / 1000.0;
    double absorbance = -log10(transmittance);
    double absorption_constant = (double)config->absorption_constant_x1000 / 1000.0;

    double concentration_pct_x1000 = (absorbance / absorption_constant) * 1000.0;
    uint32_t concentration = (uint32_t)(concentration_pct_x1000 + 0.5);

    if (config->max_valid_concentration_pct_x1000 != 0 && concentration > config->max_valid_concentration_pct_x1000)
        return OPTIC_ERROR_OUT_OF_RANGE;

    out->concentration_pct_x1000 = concentration;

    return OPTIC_OK;
}
