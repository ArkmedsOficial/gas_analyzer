#include "unity.h"
#include "hwi_timer.h"
#include "mock_bsp_timer.h"

void setUp(void) {}

void tearDown(void) {}

void test_hwi_timer_init_should_return_error_when_config_is_null(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_init(0, NULL));
}

void test_hwi_timer_init_should_return_error_when_frequency_is_zero(void)
{
    hwi_timer_config_t config = {.frequency_hz = 0, .mode = HWI_TIMER_MODE_PERIODIC};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_init(0, &config));
}

void test_hwi_timer_init_should_return_error_when_frequency_exceeds_max(void)
{
    hwi_timer_config_t config = {.frequency_hz = HWI_TIMER_MAX_FREQUENCY_HZ + 1, .mode = HWI_TIMER_MODE_PERIODIC};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_init(0, &config));
}

void test_hwi_timer_init_should_return_error_when_mode_is_invalid(void)
{
    hwi_timer_config_t config = {.frequency_hz = 1000, .mode = HWI_TIMER_MODE_COUNT};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_init(0, &config));
}

void test_hwi_timer_init_should_return_error_when_timer_id_is_out_of_range(void)
{
    hwi_timer_config_t config = {.frequency_hz = 1000, .mode = HWI_TIMER_MODE_PERIODIC};

    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_init(HWI_TIMER_MAX_ID, &config));
}

void test_hwi_timer_init_should_forward_valid_config_to_bsp(void)
{
    hwi_timer_config_t config = {.frequency_hz = 1000, .mode = HWI_TIMER_MODE_PERIODIC};

    bsp_timer_init_ExpectAndReturn(0, &config, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_timer_init(0, &config));
}

void test_hwi_timer_deinit_should_return_error_when_timer_id_is_out_of_range(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_deinit(HWI_TIMER_MAX_ID));
}

void test_hwi_timer_deinit_should_forward_to_bsp(void)
{
    bsp_timer_deinit_ExpectAndReturn(0, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_timer_deinit(0));
}

void test_hwi_timer_start_should_return_error_when_timer_id_is_out_of_range(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_start(HWI_TIMER_MAX_ID));
}

void test_hwi_timer_start_should_forward_to_bsp(void)
{
    bsp_timer_start_ExpectAndReturn(0, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_timer_start(0));
}

void test_hwi_timer_stop_should_return_error_when_timer_id_is_out_of_range(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_stop(HWI_TIMER_MAX_ID));
}

void test_hwi_timer_stop_should_forward_to_bsp(void)
{
    bsp_timer_stop_ExpectAndReturn(0, HWI_OK);

    TEST_ASSERT_EQUAL(HWI_OK, hwi_timer_stop(0));
}

void test_hwi_timer_register_callback_should_return_error_when_timer_id_is_out_of_range(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_register_callback(HWI_TIMER_MAX_ID, (hwi_timer_callback_t)0x1));
}

void test_hwi_timer_register_callback_should_return_error_when_callback_is_null(void)
{
    TEST_ASSERT_EQUAL(HWI_ERROR, hwi_timer_register_callback(0, NULL));
}

static uint8_t callback_execution_count = 0;

static void dummy_callback(void)
{
    callback_execution_count++;
}

/**
 * @brief The ISR dispatch mechanism: the BSP's real interrupt handler
 * (once implemented) calls hwi_timer_isr, which must invoke whatever
 * callback the HAL user registered for that timer.
 */
void test_hwi_timer_isr_should_invoke_registered_callback(void)
{
    callback_execution_count = 0;
    TEST_ASSERT_EQUAL(HWI_OK, hwi_timer_register_callback(0, dummy_callback));

    hwi_timer_isr(0);

    TEST_ASSERT_EQUAL_UINT8(1, callback_execution_count);
}

void test_hwi_timer_isr_should_not_crash_when_timer_id_is_out_of_range(void)
{
    hwi_timer_isr(HWI_TIMER_MAX_ID);
}

void test_hwi_timer_isr_should_not_crash_when_no_callback_is_registered(void)
{
    hwi_timer_isr(1);
}

/**
 * @brief Deinit clears the registered callback: the ISR must not keep
 * invoking a stale callback for a timer that was torn down.
 */
void test_hwi_timer_deinit_should_clear_registered_callback(void)
{
    callback_execution_count = 0;
    TEST_ASSERT_EQUAL(HWI_OK, hwi_timer_register_callback(0, dummy_callback));

    bsp_timer_deinit_ExpectAndReturn(0, HWI_OK);
    TEST_ASSERT_EQUAL(HWI_OK, hwi_timer_deinit(0));

    hwi_timer_isr(0);

    TEST_ASSERT_EQUAL_UINT8(0, callback_execution_count);
}
