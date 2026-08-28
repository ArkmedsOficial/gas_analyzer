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
