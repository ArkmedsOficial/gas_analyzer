#include "unity.h"
#include "optic_concentration.h"

void setUp(void) {}

void tearDown(void) {}

void test_optic_concentration_calculate_should_return_error_when_config_is_null(void)
{
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NULL_POINTER, optic_concentration_calculate(NULL, 500, &result));
}

void test_optic_concentration_calculate_should_return_error_when_result_is_null(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50};

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NULL_POINTER, optic_concentration_calculate(&config, 500, NULL));
}

void test_optic_concentration_calculate_should_return_error_when_absorption_constant_is_zero(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 0};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_concentration_calculate(&config, 500, &result));
}

void test_optic_concentration_calculate_should_return_error_when_signal_ratio_is_zero(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_concentration_calculate(&config, 0, &result));
}

void test_optic_concentration_calculate_should_return_error_when_signal_ratio_exceeds_reference(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_concentration_calculate(&config, 1001, &result));
}

/**
 * @brief Full transmittance (no attenuation) means no gas is present.
 */
void test_optic_concentration_calculate_should_return_zero_when_transmittance_is_full(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 1000, &result));
    TEST_ASSERT_UINT32_WITHIN(1, 0, result.concentration_pct_x1000);
}

/**
 * @brief Concentration must increase as the measured signal is increasingly
 * attenuated (lower signal_ratio_x1000), following Beer-Lambert:
 * concentration = -log10(T) / absorption_constant. With an absorption
 * constant of 0.05 absorbance/%, T=0.1 (10x attenuation) corresponds to
 * exactly 20% and T=0.01 (100x attenuation) to exactly 40%.
 */
void test_optic_concentration_calculate_should_increase_with_signal_attenuation(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50};
    optic_concentration_result_t result_low_attenuation;
    optic_concentration_result_t result_high_attenuation;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 100, &result_low_attenuation));
    TEST_ASSERT_UINT32_WITHIN(1, 20000, result_low_attenuation.concentration_pct_x1000);

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 10, &result_high_attenuation));
    TEST_ASSERT_UINT32_WITHIN(1, 40000, result_high_attenuation.concentration_pct_x1000);

    TEST_ASSERT_GREATER_THAN_UINT32(result_low_attenuation.concentration_pct_x1000, result_high_attenuation.concentration_pct_x1000);
}

/**
 * @brief The same signal ratio measured through a channel/agent pairing
 * with a different (configurable) absorption constant must yield a
 * different concentration: a higher absorption constant means the same
 * absorbance corresponds to a lower concentration.
 */
void test_optic_concentration_calculate_should_depend_on_configured_absorption_constant(void)
{
    optic_concentration_config_t weaker_absorption = {.absorption_constant_x1000 = 50};
    optic_concentration_config_t stronger_absorption = {.absorption_constant_x1000 = 100};
    optic_concentration_result_t result_weaker;
    optic_concentration_result_t result_stronger;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&weaker_absorption, 100, &result_weaker));
    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&stronger_absorption, 100, &result_stronger));

    TEST_ASSERT_UINT32_WITHIN(1, 20000, result_weaker.concentration_pct_x1000);
    TEST_ASSERT_UINT32_WITHIN(1, 10000, result_stronger.concentration_pct_x1000);
}

/**
 * @brief Lower range extreme required by DD-1097: a channel/agent pairing
 * with absorption_constant_x1000=48 and a signal_ratio_x1000 of 989
 * (T=0.989) corresponds to exactly 0.1% concentration.
 */
void test_optic_concentration_calculate_should_compute_lower_range_extreme_of_0_1_percent(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 48};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 989, &result));
    TEST_ASSERT_EQUAL_UINT32(100, result.concentration_pct_x1000);
}

/**
 * @brief Upper range extreme required by DD-1097: with the same absorption
 * constant used elsewhere (0.05 absorbance/%), a signal_ratio_x1000 of 100
 * (T=0.1) corresponds to exactly 20% concentration.
 */
void test_optic_concentration_calculate_should_compute_upper_range_extreme_of_20_percent(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 100, &result));
    TEST_ASSERT_EQUAL_UINT32(20000, result.concentration_pct_x1000);
}

/**
 * @brief Repeating the exact same input conditions must always produce the
 * exact same result: the calculation has no hidden/global state.
 */
void test_optic_concentration_calculate_should_be_repeatable_for_identical_input(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50};
    optic_concentration_result_t first_result;
    optic_concentration_result_t second_result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 250, &first_result));
    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 250, &second_result));

    TEST_ASSERT_EQUAL_UINT32(first_result.concentration_pct_x1000, second_result.concentration_pct_x1000);
}

/**
 * @brief The chamber/agent pairing is characterized by a maximum physically
 * valid concentration; a calculated value above it means the reading is out
 * of range (e.g. sensor drift, wrong agent assumed, or a fault upstream).
 */
void test_optic_concentration_calculate_should_detect_out_of_range_when_concentration_exceeds_configured_max(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50, .max_valid_concentration_pct_x1000 = 25000};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_OUT_OF_RANGE, optic_concentration_calculate(&config, 10, &result));
}

void test_optic_concentration_calculate_should_return_ok_when_concentration_is_within_configured_max(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50, .max_valid_concentration_pct_x1000 = 25000};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 100, &result));
    TEST_ASSERT_EQUAL_UINT32(20000, result.concentration_pct_x1000);
}

/**
 * @brief A concentration exactly at the configured maximum is still valid;
 * only exceeding it is out of range.
 */
void test_optic_concentration_calculate_should_return_ok_when_concentration_equals_configured_max(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50, .max_valid_concentration_pct_x1000 = 20000};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 100, &result));
    TEST_ASSERT_EQUAL_UINT32(20000, result.concentration_pct_x1000);
}

/**
 * @brief A max_valid_concentration_pct_x1000 of 0 means the range limit is
 * not configured, so the check is disabled.
 */
void test_optic_concentration_calculate_should_not_check_range_when_max_is_disabled(void)
{
    optic_concentration_config_t config = {.absorption_constant_x1000 = 50, .max_valid_concentration_pct_x1000 = 0};
    optic_concentration_result_t result;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_concentration_calculate(&config, 10, &result));
    TEST_ASSERT_EQUAL_UINT32(40000, result.concentration_pct_x1000);
}
