#include <math.h>
#include <stddef.h>

#include "optic_agent.h"

optic_status_t optic_agent_identify(const optic_agent_config_t *config,
                                     uint16_t channel_a_signal_ratio_x1000,
                                     uint16_t channel_b_signal_ratio_x1000,
                                     optic_agent_t *out_agent)
{
    if (config == NULL || out_agent == NULL)
        return OPTIC_ERROR_NULL_POINTER;

    if (channel_a_signal_ratio_x1000 == 0 || channel_a_signal_ratio_x1000 > 1000)
        return OPTIC_ERROR_INVALID_PARAM;

    if (channel_b_signal_ratio_x1000 == 0 || channel_b_signal_ratio_x1000 > 1000)
        return OPTIC_ERROR_INVALID_PARAM;

    double absorbance_a = -log10((double)channel_a_signal_ratio_x1000 / 1000.0);
    double absorbance_b = -log10((double)channel_b_signal_ratio_x1000 / 1000.0);

    if (absorbance_b == 0.0)
        return OPTIC_ERROR_INVALID_PARAM;

    uint32_t ratio_ab_x1000 = (uint32_t)((absorbance_a / absorbance_b) * 1000.0 + 0.5);

    *out_agent = OPTIC_AGENT_UNKNOWN;

    if (config->profiles != NULL) {
        for (uint8_t i = 0; i < config->profile_count; i++) {
            if (ratio_ab_x1000 >= config->profiles[i].min_ratio_x1000 &&
                ratio_ab_x1000 <= config->profiles[i].max_ratio_x1000) {
                *out_agent = config->profiles[i].agent;
                break;
            }
        }
    }

    return OPTIC_OK;
}
