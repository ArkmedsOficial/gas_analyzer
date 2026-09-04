#include "unity.h"
#include "qgc_sensor.h"
#include "mock_hwi_adc.h"

void setUp(void) {}

void tearDown(void) {}

void test_qgc_sensor_init_should_return_error_when_sensor_is_null(void)
{
    qgc_sensor_config_t config = {.type = QGC_SENSOR_TYPE_DUAL};

    TEST_ASSERT_EQUAL(HWI_ERROR, qgc_sensor_init(NULL, &config));
}

void test_qgc_sensor_init_should_return_error_when_config_is_null(void)
{
    qgc_sensor_t sensor;

    TEST_ASSERT_EQUAL(HWI_ERROR, qgc_sensor_init(&sensor, NULL));
}

/**
 * @brief The DUAL sensor (USEQGCDAANA100) has 2 channels: init must
 * configure exactly those 2 ADC channels, mapped from board wiring
 * (adc_channels), not hardcoded.
 */
void test_qgc_sensor_init_should_configure_adc_for_dual_sensor(void)
{
    qgc_sensor_t sensor;
    qgc_sensor_config_t config = {
        .type = QGC_SENSOR_TYPE_DUAL,
        .adc_id = 0,
        .adc_channels = {2, 5},
        .resolution = HWI_ADC_RESOLUTION_12BIT,
    };
    hwi_adc_config_t expected_adc_config = {0};
    expected_adc_config.resolution = HWI_ADC_RESOLUTION_12BIT;
    expected_adc_config.channels[0] = 2;
    expected_adc_config.channels[1] = 5;
    expected_adc_config.channel_count = 2;

    hwi_adc_init_ExpectAndReturn(0, &expected_adc_config, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, qgc_sensor_init(&sensor, &config));
    TEST_ASSERT_EQUAL_UINT8(2, qgc_sensor_channel_count(&sensor));
}

/**
 * @brief The QUAD sensor (USEQGCQAAN2100) has 4 channels.
 */
void test_qgc_sensor_init_should_configure_adc_for_quad_sensor(void)
{
    qgc_sensor_t sensor;
    qgc_sensor_config_t config = {
        .type = QGC_SENSOR_TYPE_QUAD,
        .adc_id = 1,
        .adc_channels = {0, 1, 2, 3},
        .resolution = HWI_ADC_RESOLUTION_12BIT,
    };
    hwi_adc_config_t expected_adc_config = {0};
    expected_adc_config.resolution = HWI_ADC_RESOLUTION_12BIT;
    expected_adc_config.channels[0] = 0;
    expected_adc_config.channels[1] = 1;
    expected_adc_config.channels[2] = 2;
    expected_adc_config.channels[3] = 3;
    expected_adc_config.channel_count = 4;

    hwi_adc_init_ExpectAndReturn(1, &expected_adc_config, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, qgc_sensor_init(&sensor, &config));
    TEST_ASSERT_EQUAL_UINT8(4, qgc_sensor_channel_count(&sensor));
}

void test_qgc_sensor_init_should_propagate_hwi_adc_failure(void)
{
    qgc_sensor_t sensor;
    qgc_sensor_config_t config = {.type = QGC_SENSOR_TYPE_DUAL, .adc_channels = {2, 5}};

    hwi_adc_init_IgnoreAndReturn(HWI_ERROR);

    TEST_ASSERT_EQUAL(HWI_ERROR, qgc_sensor_init(&sensor, &config));
}

void test_qgc_sensor_channel_count_should_return_zero_when_sensor_is_null(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, qgc_sensor_channel_count(NULL));
}

void test_qgc_sensor_read_channel_mv_should_return_error_when_sensor_is_null(void)
{
    uint16_t voltage_mv;

    TEST_ASSERT_EQUAL(HWI_ERROR, qgc_sensor_read_channel_mv(NULL, 0, &voltage_mv));
}

void test_qgc_sensor_read_channel_mv_should_return_error_when_voltage_pointer_is_null(void)
{
    qgc_sensor_t sensor;
    qgc_sensor_config_t config = {.type = QGC_SENSOR_TYPE_DUAL, .adc_channels = {2, 5}};
    hwi_adc_init_IgnoreAndReturn(HWI_OK);
    qgc_sensor_init(&sensor, &config);

    TEST_ASSERT_EQUAL(HWI_ERROR, qgc_sensor_read_channel_mv(&sensor, 0, NULL));
}

void test_qgc_sensor_read_channel_mv_should_return_error_when_channel_index_is_out_of_range(void)
{
    qgc_sensor_t sensor;
    qgc_sensor_config_t config = {.type = QGC_SENSOR_TYPE_DUAL, .adc_channels = {2, 5}};
    uint16_t voltage_mv;
    hwi_adc_init_IgnoreAndReturn(HWI_OK);
    qgc_sensor_init(&sensor, &config);

    TEST_ASSERT_EQUAL(HWI_ERROR, qgc_sensor_read_channel_mv(&sensor, 2, &voltage_mv));
}

/**
 * @brief Reading sensor channel 1 must read the physical ADC channel it is
 * wired to (5, per the board mapping in setUp), not the channel index (1)
 * itself: the two are only coincidentally different here on purpose, to
 * prove the indirection is real.
 */
void test_qgc_sensor_read_channel_mv_should_read_the_mapped_adc_channel(void)
{
    qgc_sensor_t sensor;
    qgc_sensor_config_t config = {.type = QGC_SENSOR_TYPE_DUAL, .adc_id = 0, .adc_channels = {2, 5}};
    uint16_t voltage_mv = 0;
    uint16_t bsp_value_mv = 1234;

    hwi_adc_init_IgnoreAndReturn(HWI_OK);
    qgc_sensor_init(&sensor, &config);

    hwi_adc_read_mv_ExpectAndReturn(0, 5, NULL, HWI_OK);
    hwi_adc_read_mv_IgnoreArg_voltage_mv();
    hwi_adc_read_mv_ReturnThruPtr_voltage_mv(&bsp_value_mv);

    TEST_ASSERT_EQUAL(HWI_OK, qgc_sensor_read_channel_mv(&sensor, 1, &voltage_mv));
    TEST_ASSERT_EQUAL_UINT16(1234, voltage_mv);
}

void test_qgc_sensor_read_channel_mv_should_propagate_hwi_adc_failure(void)
{
    qgc_sensor_t sensor;
    qgc_sensor_config_t config = {.type = QGC_SENSOR_TYPE_DUAL, .adc_channels = {2, 5}};
    uint16_t voltage_mv;

    hwi_adc_init_IgnoreAndReturn(HWI_OK);
    qgc_sensor_init(&sensor, &config);

    hwi_adc_read_mv_IgnoreAndReturn(HWI_ERROR);

    TEST_ASSERT_EQUAL(HWI_ERROR, qgc_sensor_read_channel_mv(&sensor, 0, &voltage_mv));
}
