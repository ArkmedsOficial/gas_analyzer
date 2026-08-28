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

    if (frame->saturation_threshold != 0 && peak_value >= frame->saturation_threshold)
        return OPTIC_ERROR_SATURATION;

    if (peak_index >= on_samples)
        return OPTIC_ERROR_OUT_OF_PHASE;

    out->peak_value = peak_value;
    out->valley_value = valley_value;
    out->amplitude = (uint16_t)(peak_value - valley_value);

    return OPTIC_OK;
}

static uint16_t optic_saturate_to_uint16(uint32_t value)
{
    return (value > UINT16_MAX) ? UINT16_MAX : (uint16_t)value;
}

optic_status_t optic_channel_apply_calibration(const optic_channel_calibration_t *calibration,
                                                uint16_t raw_amplitude,
                                                optic_channel_calibrated_result_t *out)
{
    if (calibration == NULL || out == NULL)
        return OPTIC_ERROR_NULL_POINTER;

    if (calibration->reference_amplitude == 0 || calibration->gain_calibration_x1000 == 0)
        return OPTIC_ERROR_INVALID_PARAM;

    uint32_t compensated_amplitude = ((uint32_t)raw_amplitude * calibration->gain_calibration_x1000) / 1000U;
    out->compensated_amplitude = optic_saturate_to_uint16(compensated_amplitude);

    uint32_t signal_ratio_x1000 = ((uint32_t)out->compensated_amplitude * 1000U) / calibration->reference_amplitude;
    out->signal_ratio_x1000 = optic_saturate_to_uint16(signal_ratio_x1000);

    return OPTIC_OK;
}
