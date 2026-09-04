#include "unity.h"
#include "hwi_adc.h"
#include "mock_bsp_adc.h"

void setUp(void) {}

void tearDown(void) {}

void test_hwi_adc_init_should_return_error_when_config_is_null(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_adc_init(0, NULL));
}

void test_hwi_adc_init_should_return_error_when_adc_id_is_out_of_range(void)
{
    hwi_adc_config_t config = {.resolution = HWI_ADC_RESOLUTION_12BIT, .channels = {0}, .channel_count = 1};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_adc_init(HWI_ADC_MAX_ID, &config));
}

void test_hwi_adc_init_should_return_error_when_resolution_is_invalid(void)
{
    hwi_adc_config_t config = {.resolution = HWI_ADC_RESOLUTION_COUNT, .channels = {0}, .channel_count = 1};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_adc_init(0, &config));
}

void test_hwi_adc_init_should_return_error_when_channel_count_is_zero(void)
{
    hwi_adc_config_t config = {.resolution = HWI_ADC_RESOLUTION_12BIT, .channels = {0}, .channel_count = 0};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_adc_init(0, &config));
}

void test_hwi_adc_init_should_return_error_when_channel_count_exceeds_max(void)
{
    hwi_adc_config_t config = {.resolution = HWI_ADC_RESOLUTION_12BIT, .channel_count = HWI_ADC_MAX_CHANNELS + 1};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_adc_init(0, &config));
}

void test_hwi_adc_init_should_return_error_when_a_configured_channel_is_out_of_range(void)
{
    hwi_adc_config_t config = {.resolution = HWI_ADC_RESOLUTION_12BIT, .channels = {HWI_ADC_MAX_CHANNELS}, .channel_count = 1};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_adc_init(0, &config));
}

/**
 * @brief A valid configuration is forwarded to the BSP: the HAL owns
 * parameter validation, the BSP owns the actual peripheral setup.
 */
void test_hwi_adc_init_should_forward_valid_config_to_bsp(void)
{
    hwi_adc_config_t config = {.resolution = HWI_ADC_RESOLUTION_12BIT, .channels = {0, 1}, .channel_count = 2};

    bsp_adc_init_ExpectAndReturn(0, &config, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_adc_init(0, &config));
}

void test_hwi_adc_init_should_propagate_bsp_failure(void)
{
    hwi_adc_config_t config = {.resolution = HWI_ADC_RESOLUTION_12BIT, .channels = {0}, .channel_count = 1};

    bsp_adc_init_ExpectAndReturn(0, &config, HWI_ERROR);

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_adc_init(0, &config));
}

void test_hwi_adc_deinit_should_return_error_when_adc_id_is_out_of_range(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_adc_deinit(HWI_ADC_MAX_ID));
}

void test_hwi_adc_deinit_should_forward_to_bsp(void)
{
    bsp_adc_deinit_ExpectAndReturn(0, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_adc_deinit(0));
}

void test_hwi_adc_read_mv_should_return_value_read_from_bsp(void)
{
    int bsp_value_mv = 1650;
    uint16_t voltage_mv = 0;

    bsp_adc_read_mv_ExpectAndReturn(0, 3, NULL, HWI_OK);
    bsp_adc_read_mv_IgnoreArg_out_mv();
    bsp_adc_read_mv_ReturnThruPtr_out_mv(&bsp_value_mv);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_adc_read_mv(0, 3, &voltage_mv));
    TEST_ASSERT_EQUAL_UINT16(1650, voltage_mv);
}

void test_hwi_adc_read_mv_should_propagate_bsp_failure(void)
{
    uint16_t voltage_mv = 0;

    bsp_adc_read_mv_ExpectAndReturn(0, 3, NULL, HWI_ERROR);
    bsp_adc_read_mv_IgnoreArg_out_mv();

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_adc_read_mv(0, 3, &voltage_mv));
}
