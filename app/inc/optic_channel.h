#ifndef OPTIC_CHANNEL_H
#define OPTIC_CHANNEL_H

#include <stdint.h>

typedef enum {
    OPTIC_OK = 0,
    OPTIC_ERROR_NULL_POINTER,
    OPTIC_ERROR_INVALID_PARAM,
    OPTIC_ERROR_OUT_OF_PHASE
} optic_status_t;

/* Raw sample buffer for a single detector channel, spanning at least one
 * full IR source drive cycle. Acquisition (ADC/GPIO/timer) is out of scope
 * for this layer; the buffer is expected to already be in engineering units
 * (or raw counts) at a known, fixed sample rate. */
typedef struct {
    const uint16_t *samples;
    uint16_t sample_count;
    uint32_t sample_rate_hz;
} optic_channel_frame_t;

/* Expected timing of the IR emitter drive cycle, used to locate the
 * source-on (peak) and source-off (valley) windows within the frame and to
 * detect a signal that is out of phase with the drive. */
typedef struct {
    uint32_t ir_period_us;
    uint32_t ir_duty_on_us;
} optic_ir_drive_t;

typedef struct {
    uint16_t peak_value;
    uint16_t valley_value;
    uint16_t amplitude;
} optic_channel_result_t;

optic_status_t optic_channel_process(const optic_channel_frame_t *frame,
                                      const optic_ir_drive_t *drive,
                                      optic_channel_result_t *out);

/* Per-channel calibration, dependent on the physical characterization of
 * the sensor and must remain configurable (never hardcoded into the
 * processing logic). */
typedef struct {
    /* Amplitude captured under a known reference condition (e.g. zero-gas),
     * i.e. this channel's I0. */
    uint16_t reference_amplitude;
    /* Multiplicative correction for this channel's gain deviation, scaled
     * by 1000 (1000 = no correction). */
    uint16_t gain_calibration_x1000;
} optic_channel_calibration_t;

typedef struct {
    /* Raw amplitude after gain-deviation correction. */
    uint16_t compensated_amplitude;
    /* compensated_amplitude / reference_amplitude (I/I0), scaled by 1000. */
    uint16_t signal_ratio_x1000;
} optic_channel_calibrated_result_t;

optic_status_t optic_channel_apply_calibration(const optic_channel_calibration_t *calibration,
                                                uint16_t raw_amplitude,
                                                optic_channel_calibrated_result_t *out);

#endif /* OPTIC_CHANNEL_H */
