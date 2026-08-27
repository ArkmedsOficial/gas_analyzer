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
