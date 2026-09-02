#ifndef OPTIC_AGENT_H
#define OPTIC_AGENT_H

#include <stdint.h>

#include "optic_channel.h"

typedef enum {
    OPTIC_AGENT_UNKNOWN = 0,
    OPTIC_AGENT_ISOFLURANE,
    OPTIC_AGENT_DESFLURANE
} optic_agent_t;

/* Expected channel-to-channel absorbance ratio signature for one agent.
 * Dependent on the sensor's spectral characterization (which two channels
 * are compared, and their expected ratio band) and must remain
 * configurable rather than hardcoded.
 *
 * By Beer-Lambert, absorbance = absorption_constant * concentration, so
 * absorbance_a / absorbance_b = absorption_constant_a / absorption_constant_b
 * regardless of concentration -- this is why a channel ratio identifies the
 * agent independently of how much of it is present. The same pair of
 * channels works for a 2-channel (DUAL) or 4-channel (QUAD) sensor: the
 * caller selects which two channels to compare. */
typedef struct {
    optic_agent_t agent;
    uint32_t min_ratio_x1000;
    uint32_t max_ratio_x1000;
} optic_agent_profile_t;

typedef struct {
    const optic_agent_profile_t *profiles;
    uint8_t profile_count;
} optic_agent_config_t;

optic_status_t optic_agent_identify(const optic_agent_config_t *config,
                                     uint16_t channel_a_signal_ratio_x1000,
                                     uint16_t channel_b_signal_ratio_x1000,
                                     optic_agent_t *out_agent);

#endif /* OPTIC_AGENT_H */
