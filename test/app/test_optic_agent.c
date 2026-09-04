#include "unity.h"
#include "optic_agent.h"

static const optic_agent_profile_t agent_profiles[] = {
    {.agent = OPTIC_AGENT_ISOFLURANE, .min_ratio_x1000 = 1500, .max_ratio_x1000 = 2500},
    {.agent = OPTIC_AGENT_DESFLURANE, .min_ratio_x1000 = 250, .max_ratio_x1000 = 750},
};

static const optic_agent_config_t agent_config = {
    .profiles = agent_profiles,
    .profile_count = sizeof(agent_profiles) / sizeof(agent_profiles[0]),
};

void setUp(void) {}

void tearDown(void) {}

void test_optic_agent_identify_should_return_error_when_config_is_null(void)
{
    optic_agent_t agent;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_NULL_POINTER, optic_agent_identify(NULL, 10, 100, &agent));
}

void test_optic_agent_identify_should_return_error_when_out_agent_is_null(void)
{
    TEST_ASSERT_EQUAL(OPTIC_ERROR_NULL_POINTER, optic_agent_identify(&agent_config, 10, 100, NULL));
}

void test_optic_agent_identify_should_return_error_when_channel_a_ratio_is_out_of_range(void)
{
    optic_agent_t agent;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_agent_identify(&agent_config, 0, 100, &agent));
    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_agent_identify(&agent_config, 1001, 100, &agent));
}

void test_optic_agent_identify_should_return_error_when_channel_b_ratio_is_out_of_range(void)
{
    optic_agent_t agent;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_agent_identify(&agent_config, 10, 0, &agent));
    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_agent_identify(&agent_config, 10, 1001, &agent));
}

/**
 * @brief A channel_b with full transmittance (T=1, no attenuation) has zero
 * absorbance, so the channel ratio (absorbance_a/absorbance_b) is
 * undefined.
 */
void test_optic_agent_identify_should_return_error_when_channel_b_has_no_absorbance(void)
{
    optic_agent_t agent;

    TEST_ASSERT_EQUAL(OPTIC_ERROR_INVALID_PARAM, optic_agent_identify(&agent_config, 10, 1000, &agent));
}

/**
 * @brief Isoflurane signature: channel A absorbs twice as much as channel B
 * (T=0.01 => absorbance 2, T=0.1 => absorbance 1, ratio 2.0).
 */
void test_optic_agent_identify_should_identify_isoflurane_from_channel_ratio(void)
{
    optic_agent_t agent;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_agent_identify(&agent_config, 10, 100, &agent));
    TEST_ASSERT_EQUAL(OPTIC_AGENT_ISOFLURANE, agent);
}

/**
 * @brief Desflurane signature: the same two channel readings as the
 * isoflurane case, but with the dominant channel swapped (ratio 0.5
 * instead of 2.0), proving the two agents are discriminated by which
 * channel absorbs more, not just by signal magnitude.
 */
void test_optic_agent_identify_should_identify_desflurane_from_channel_ratio(void)
{
    optic_agent_t agent;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_agent_identify(&agent_config, 100, 10, &agent));
    TEST_ASSERT_EQUAL(OPTIC_AGENT_DESFLURANE, agent);
}

/**
 * @brief A channel ratio matching no configured profile is reported as
 * OPTIC_AGENT_UNKNOWN, not as an error: e.g. no gas present, or an agent
 * outside the configured set.
 */
void test_optic_agent_identify_should_return_unknown_when_ratio_matches_no_profile(void)
{
    optic_agent_t agent;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_agent_identify(&agent_config, 100, 100, &agent));
    TEST_ASSERT_EQUAL(OPTIC_AGENT_UNKNOWN, agent);
}

/**
 * @brief Agent identification is concentration-independent: doubling the
 * attenuation of both channels (i.e. a different gas concentration) keeps
 * the same absorbance ratio and therefore the same identified agent.
 */
void test_optic_agent_identify_should_be_independent_of_concentration(void)
{
    optic_agent_t agent_at_lower_concentration;
    optic_agent_t agent_at_higher_concentration;

    TEST_ASSERT_EQUAL(OPTIC_OK, optic_agent_identify(&agent_config, 100, 316, &agent_at_lower_concentration));
    TEST_ASSERT_EQUAL(OPTIC_OK, optic_agent_identify(&agent_config, 10, 100, &agent_at_higher_concentration));

    TEST_ASSERT_EQUAL(OPTIC_AGENT_ISOFLURANE, agent_at_lower_concentration);
    TEST_ASSERT_EQUAL(OPTIC_AGENT_ISOFLURANE, agent_at_higher_concentration);
}
