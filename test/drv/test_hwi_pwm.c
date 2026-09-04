#include "unity.h"
#include "hwi_pwm.h"
#include "mock_bsp_pwm.h"

void setUp(void) {}

void tearDown(void) {}

void test_hwi_pwm_init_should_return_error_when_pwm_is_null(void)
{
    hwi_pwm_config_t config = {.frequency_hz = 1000, .duty_percent = 50};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_init(NULL, &config));
}

void test_hwi_pwm_init_should_return_error_when_config_is_null(void)
{
    hwi_pwm_t pwm = {0};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_init(&pwm, NULL));
}

void test_hwi_pwm_init_should_return_error_when_frequency_is_zero(void)
{
    hwi_pwm_t pwm = {0};
    hwi_pwm_config_t config = {.frequency_hz = 0, .duty_percent = 50};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_init(&pwm, &config));
}

void test_hwi_pwm_init_should_return_error_when_duty_percent_exceeds_100(void)
{
    hwi_pwm_t pwm = {0};
    hwi_pwm_config_t config = {.frequency_hz = 1000, .duty_percent = 101};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_init(&pwm, &config));
}

void test_hwi_pwm_init_should_copy_config_and_return_success(void)
{
    hwi_pwm_t pwm = {0};
    hwi_pwm_config_t config = {.port = 0, .pin = 21, .frequency_hz = 5000, .duty_percent = 75, .timer_id = 1};
    void *expected_handle = (void *)0x1234;

    bsp_pwm_init_ExpectAndReturn(&config, NULL, HWI_OK);
    bsp_pwm_init_IgnoreArg_out_handle();
    bsp_pwm_init_ReturnThruPtr_out_handle(&expected_handle);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_pwm_init(&pwm, &config));
    TEST_ASSERT_EQUAL(config.port, pwm.port);
    TEST_ASSERT_EQUAL(config.pin, pwm.pin);
    TEST_ASSERT_EQUAL(config.frequency_hz, pwm.frequency_hz);
    TEST_ASSERT_EQUAL(config.duty_percent, pwm.duty_percent);
    TEST_ASSERT_EQUAL(config.timer_id, pwm.timer_id);
    TEST_ASSERT_EQUAL_PTR(expected_handle, pwm.bsp_handle);
}

void test_hwi_pwm_init_should_propagate_bsp_failure(void)
{
    hwi_pwm_t pwm = {0};
    hwi_pwm_config_t config = {.frequency_hz = 1000, .duty_percent = 50};

    bsp_pwm_init_ExpectAndReturn(&config, NULL, HWI_ERROR);
    bsp_pwm_init_IgnoreArg_out_handle();

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_init(&pwm, &config));
}

void test_hwi_pwm_deinit_should_return_error_when_pwm_is_null(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_deinit(NULL));
}

void test_hwi_pwm_deinit_should_return_ok_when_handle_is_null(void)
{
    hwi_pwm_t pwm = {0};

    TEST_ASSERT_EQUAL(HWI_OK, hwi_pwm_deinit(&pwm));
}

void test_hwi_pwm_deinit_should_forward_to_bsp_and_clear_handle(void)
{
    hwi_pwm_t pwm = {.bsp_handle = (void *)0x1234};

    bsp_pwm_deinit_ExpectAndReturn(pwm.bsp_handle, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_pwm_deinit(&pwm));
    TEST_ASSERT_NULL(pwm.bsp_handle);
}

void test_hwi_pwm_start_should_return_error_when_pwm_is_null(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_start(NULL));
}

void test_hwi_pwm_start_should_return_error_when_not_initialized(void)
{
    hwi_pwm_t pwm = {0};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_start(&pwm));
}

void test_hwi_pwm_start_should_forward_to_bsp(void)
{
    hwi_pwm_t pwm = {.bsp_handle = (void *)0x1234};

    bsp_pwm_start_ExpectAndReturn(pwm.bsp_handle, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_pwm_start(&pwm));
}

void test_hwi_pwm_stop_should_return_error_when_pwm_is_null(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_stop(NULL));
}

void test_hwi_pwm_stop_should_return_error_when_not_initialized(void)
{
    hwi_pwm_t pwm = {0};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_stop(&pwm));
}

void test_hwi_pwm_stop_should_forward_to_bsp(void)
{
    hwi_pwm_t pwm = {.bsp_handle = (void *)0x1234};

    bsp_pwm_stop_ExpectAndReturn(pwm.bsp_handle, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_pwm_stop(&pwm));
}

void test_hwi_pwm_set_duty_should_return_error_when_pwm_is_null(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_set_duty(NULL, 50));
}

void test_hwi_pwm_set_duty_should_return_error_when_not_initialized(void)
{
    hwi_pwm_t pwm = {0};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_set_duty(&pwm, 50));
}

void test_hwi_pwm_set_duty_should_return_error_when_duty_percent_exceeds_100(void)
{
    hwi_pwm_t pwm = {.bsp_handle = (void *)0x1234};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_set_duty(&pwm, 101));
}

void test_hwi_pwm_set_duty_should_update_cached_value_and_forward_to_bsp(void)
{
    hwi_pwm_t pwm = {.bsp_handle = (void *)0x1234, .duty_percent = 0};

    bsp_pwm_set_duty_ExpectAndReturn(pwm.bsp_handle, 42, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_pwm_set_duty(&pwm, 42));
    TEST_ASSERT_EQUAL(42, pwm.duty_percent);
}

void test_hwi_pwm_set_frequency_should_return_error_when_pwm_is_null(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_set_frequency(NULL, 1000));
}

void test_hwi_pwm_set_frequency_should_return_error_when_not_initialized(void)
{
    hwi_pwm_t pwm = {0};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_set_frequency(&pwm, 1000));
}

void test_hwi_pwm_set_frequency_should_return_error_when_frequency_is_zero(void)
{
    hwi_pwm_t pwm = {.bsp_handle = (void *)0x1234};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_pwm_set_frequency(&pwm, 0));
}

void test_hwi_pwm_set_frequency_should_update_cached_value_and_forward_to_bsp(void)
{
    hwi_pwm_t pwm = {.bsp_handle = (void *)0x1234, .frequency_hz = 0};

    bsp_pwm_set_frequency_ExpectAndReturn(pwm.bsp_handle, 8000, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_pwm_set_frequency(&pwm, 8000));
    TEST_ASSERT_EQUAL(8000, pwm.frequency_hz);
}
