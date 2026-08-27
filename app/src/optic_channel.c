#include <stddef.h>

#include "optic_channel.h"

optic_status_t optic_channel_process(const optic_channel_frame_t *frame,
                                      const optic_ir_drive_t *drive,
                                      optic_channel_result_t *out)
{
    if (frame == NULL || drive == NULL || out == NULL)
        return OPTIC_ERROR_NULL_POINTER;

    if (frame->samples == NULL || frame->sample_count == 0 || frame->sample_rate_hz == 0)
        return OPTIC_ERROR_INVALID_PARAM;

    if (drive->ir_duty_on_us == 0 || drive->ir_duty_on_us >= drive->ir_period_us)
        return OPTIC_ERROR_INVALID_PARAM;

    uint32_t on_samples = (uint32_t)(((uint64_t)drive->ir_duty_on_us * frame->sample_rate_hz) / 1000000ULL);
    if (on_samples == 0 || on_samples >= frame->sample_count)
        return OPTIC_ERROR_INVALID_PARAM;

    uint16_t peak_value = frame->samples[0];
    uint16_t peak_index = 0;
    uint16_t valley_value = frame->samples[0];

    for (uint16_t i = 0; i < frame->sample_count; i++) {
        if (frame->samples[i] > peak_value) {
            peak_value = frame->samples[i];
            peak_index = i;
        }
        if (frame->samples[i] < valley_value)
            valley_value = frame->samples[i];
    }

    if (peak_index >= on_samples)
        return OPTIC_ERROR_OUT_OF_PHASE;

    out->peak_value = peak_value;
    out->valley_value = valley_value;
    out->amplitude = (uint16_t)(peak_value - valley_value);

    return OPTIC_OK;
}
