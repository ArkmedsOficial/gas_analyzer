#include "unity.h"
#include "optic_channel.h"

void setUp(void) {}

void tearDown(void) {}

/**
 * @brief Test that a NULL frame pointer is rejected.
 */
void test_optic_channel_process_should_return_error_when_frame_is_null(void)
{
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NULL_POINTER, optic_channel_process(NULL, &drive, &result));
}

void test_optic_channel_process_should_return_error_when_drive_is_null(void)
{
    uint16_t samples[4] = {0};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 4, .sample_rate_hz = 1000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NULL_POINTER, optic_channel_process(&frame, NULL, &result));
}

void test_optic_channel_process_should_return_error_when_result_is_null(void)
{
    uint16_t samples[4] = {0};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 4, .sample_rate_hz = 1000};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NULL_POINTER, optic_channel_process(&frame, &drive, NULL));
}

void test_optic_channel_process_should_return_error_when_samples_pointer_is_null(void)
{
    optic_channel_frame_t frame = {.samples = NULL, .sample_count = 4, .sample_rate_hz = 1000};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_channel_process(&frame, &drive, &result));
}

void test_optic_channel_process_should_return_error_when_sample_count_is_zero(void)
{
    uint16_t samples[4] = {0};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 0, .sample_rate_hz = 1000};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_channel_process(&frame, &drive, &result));
}

void test_optic_channel_process_should_return_error_when_sample_rate_is_zero(void)
{
    uint16_t samples[4] = {0};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 4, .sample_rate_hz = 0};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_channel_process(&frame, &drive, &result));
}

void test_optic_channel_process_should_return_error_when_duty_on_is_not_smaller_than_period(void)
{
    uint16_t samples[10] = {0};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 10000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_channel_process(&frame, &drive, &result));
}

/**
 * @brief Synthetic signal: source-on window (first 5 samples) carries the
 * peak, source-off window (last 5 samples) carries the valley, matching the
 * expected IR drive timing.
 */
void test_optic_channel_process_should_compute_amplitude_for_synchronized_signal(void)
{
    uint16_t samples[10] = {900, 920, 910, 905, 895, 100, 110, 90, 105, 95};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_channel_process(&frame, &drive, &result));
    TEST_ASSERT_EQUAL_UINT16(920, result.peak_value);
    TEST_ASSERT_EQUAL_UINT16(90, result.valley_value);
    TEST_ASSERT_EQUAL_UINT16(830, result.amplitude);
}

/**
 * @brief Synthetic signal: the peak falls inside the source-off window,
 * i.e. deliberately out of phase with the expected IR drive timing.
 */
void test_optic_channel_process_should_detect_out_of_phase_signal(void)
{
    uint16_t samples[10] = {100, 110, 90, 105, 95, 900, 920, 910, 905, 895};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_OUT_OF_PHASE, optic_channel_process(&frame, &drive, &result));
}

/**
 * @brief The detector/ADC is characterized by a saturation threshold; a
 * peak reaching or exceeding it means the channel is overloaded and its
 * amplitude cannot be trusted.
 */
void test_optic_channel_process_should_detect_saturation_when_peak_reaches_threshold(void)
{
    uint16_t samples[10] = {900, 920, 910, 905, 895, 100, 110, 90, 105, 95};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000, .saturation_threshold = 920};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_SATURATION, optic_channel_process(&frame, &drive, &result));
}

void test_optic_channel_process_should_return_ok_when_peak_is_below_saturation_threshold(void)
{
    uint16_t samples[10] = {900, 920, 910, 905, 895, 100, 110, 90, 105, 95};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000, .saturation_threshold = 1000};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_channel_process(&frame, &drive, &result));
    TEST_ASSERT_EQUAL_UINT16(920, result.peak_value);
}

/**
 * @brief A saturation_threshold of 0 means the channel's saturation limit
 * is not configured, so the check is disabled and no peak value flags it.
 */
void test_optic_channel_process_should_not_check_saturation_when_threshold_is_disabled(void)
{
    uint16_t samples[10] = {60000, 61000, 60500, 60200, 60100, 100, 110, 90, 105, 95};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000, .saturation_threshold = 0};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_channel_process(&frame, &drive, &result));
}

/**
 * @brief When a signal is both saturated and out of phase, saturation must
 * be reported: a pinned/overloaded reading cannot be trusted to locate a
 * meaningful peak position in the first place.
 */
void test_optic_channel_process_should_prioritize_saturation_over_out_of_phase(void)
{
    uint16_t samples[10] = {100, 110, 90, 105, 95, 900, 920, 910, 905, 895};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000, .saturation_threshold = 920};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_SATURATION, optic_channel_process(&frame, &drive, &result));
}

/**
 * @brief The detector is characterized by a minimum expected peak-to-valley
 * amplitude; an amplitude below it means the channel signal is absent (e.g.
 * disconnected sensor, obstructed optical path, or IR source not driving).
 */
void test_optic_channel_process_should_detect_no_signal_when_amplitude_is_below_threshold(void)
{
    uint16_t samples[10] = {505, 508, 506, 504, 503, 500, 502, 499, 501, 500};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000, .min_signal_threshold = 20};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NO_SIGNAL, optic_channel_process(&frame, &drive, &result));
}

void test_optic_channel_process_should_return_ok_when_amplitude_is_at_or_above_no_signal_threshold(void)
{
    uint16_t samples[10] = {510, 520, 515, 512, 508, 500, 502, 501, 503, 505};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000, .min_signal_threshold = 20};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_channel_process(&frame, &drive, &result));
    TEST_ASSERT_EQUAL_UINT16(20, result.amplitude);
}

/**
 * @brief A min_signal_threshold of 0 means the channel's no-signal limit is
 * not configured, so the check is disabled and no amplitude flags it.
 */
void test_optic_channel_process_should_not_check_no_signal_when_threshold_is_disabled(void)
{
    uint16_t samples[10] = {500, 501, 500, 500, 500, 500, 500, 500, 500, 500};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000, .min_signal_threshold = 0};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_channel_process(&frame, &drive, &result));
}

/**
 * @brief When a signal is both below the no-signal threshold and out of
 * phase, no-signal must be reported: without a reliable peak amplitude, the
 * peak's position within the frame cannot be trusted either.
 */
void test_optic_channel_process_should_prioritize_no_signal_over_out_of_phase(void)
{
    uint16_t samples[10] = {500, 502, 499, 501, 500, 505, 508, 506, 504, 503};
    optic_channel_frame_t frame = {.samples = samples, .sample_count = 10, .sample_rate_hz = 1000, .min_signal_threshold = 20};
    optic_ir_drive_t drive = {.ir_period_us = 10000, .ir_duty_on_us = 5000};
    optic_channel_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NO_SIGNAL, optic_channel_process(&frame, &drive, &result));
}

void test_optic_channel_apply_calibration_should_return_error_when_calibration_is_null(void)
{
    optic_channel_calibrated_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NULL_POINTER, optic_channel_apply_calibration(NULL, 600, &result));
}

void test_optic_channel_apply_calibration_should_return_error_when_result_is_null(void)
{
    optic_channel_calibration_t calibration = {.reference_amplitude = 1000, .gain_calibration_x1000 = 1000};

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NULL_POINTER, optic_channel_apply_calibration(&calibration, 600, NULL));
}

void test_optic_channel_apply_calibration_should_return_error_when_reference_amplitude_is_zero(void)
{
    optic_channel_calibration_t calibration = {.reference_amplitude = 0, .gain_calibration_x1000 = 1000};
    optic_channel_calibrated_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_channel_apply_calibration(&calibration, 600, &result));
}

void test_optic_channel_apply_calibration_should_return_error_when_gain_calibration_is_zero(void)
{
    optic_channel_calibration_t calibration = {.reference_amplitude = 1000, .gain_calibration_x1000 = 0};
    optic_channel_calibrated_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_channel_apply_calibration(&calibration, 600, &result));
}

/**
 * @brief Reference signal fully applied: a raw amplitude equal to the
 * channel's reference (established under a known, gas-free condition)
 * yields full transmittance (1000/1000 = 100%).
 */
void test_optic_channel_apply_calibration_should_apply_reference_signal_at_full_transmittance(void)
{
    optic_channel_calibration_t calibration = {.reference_amplitude = 1000, .gain_calibration_x1000 = 1000};
    optic_channel_calibrated_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_channel_apply_calibration(&calibration, 1000, &result));
    TEST_ASSERT_EQUAL_UINT16(1000, result.compensated_amplitude);
    TEST_ASSERT_EQUAL_UINT16(1000, result.signal_ratio_x1000);
}

/**
 * @brief Reference signal applied to an attenuated reading (gas present):
 * 600 out of a 1000 reference is 60% transmittance.
 */
void test_optic_channel_apply_calibration_should_apply_reference_signal_at_partial_transmittance(void)
{
    optic_channel_calibration_t calibration = {.reference_amplitude = 1000, .gain_calibration_x1000 = 1000};
    optic_channel_calibrated_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_channel_apply_calibration(&calibration, 600, &result));
    TEST_ASSERT_EQUAL_UINT16(600, result.compensated_amplitude);
    TEST_ASSERT_EQUAL_UINT16(600, result.signal_ratio_x1000);
}

/**
 * @brief Two channels with the same true physical amplitude (600) but
 * different hardware gain must produce the same compensated amplitude and
 * signal ratio once the per-channel gain deviation is compensated: a +25%
 * gain deviation (raw 750) is corrected by an 800/1000 factor down to 600,
 * matching a channel with no gain deviation reading 600 directly.
 */
void test_optic_channel_apply_calibration_should_compensate_gain_deviation_between_channels(void)
{
    optic_channel_calibration_t calibration_without_deviation = {.reference_amplitude = 1000, .gain_calibration_x1000 = 1000};
    optic_channel_calibration_t calibration_with_deviation = {.reference_amplitude = 1000, .gain_calibration_x1000 = 800};
    optic_channel_calibrated_result_t result_without_deviation;
    optic_channel_calibrated_result_t result_with_deviation;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_channel_apply_calibration(&calibration_without_deviation, 600, &result_without_deviation));
    TEST_ASSERT_EQUAL(OPTIC_OK, optic_channel_apply_calibration(&calibration_with_deviation, 750, &result_with_deviation));

    TEST_ASSERT_EQUAL_UINT16(result_without_deviation.compensated_amplitude, result_with_deviation.compensated_amplitude);
    TEST_ASSERT_EQUAL_UINT16(result_without_deviation.signal_ratio_x1000, result_with_deviation.signal_ratio_x1000);
}
