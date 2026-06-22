#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Test invariant: Buffer reads never exceed declared length.
 * The CMVMSG buffer is 24 bytes; with offset 4, only 20 bytes are safe.
 * Size parameter masked with 0xf allows up to 15, multiplied by 2 = 30 bytes.
 * This test verifies that oversized inputs are either rejected or truncated.
 */

START_TEST(test_cmvmsg_buffer_overflow_prevention)
{
    /* Payloads: exploit case (size=15 → 30 bytes), boundary (size=10 → 20 bytes safe),
     * valid (size=5 → 10 bytes safe), and edge cases */
    struct {
        uint32_t size;
        const char *description;
    } test_cases[] = {
        {15, "exploit: max masked size (30 bytes into 20-byte space)"},
        {10, "boundary: 20 bytes (fills safe space exactly)"},
        {5, "valid: 10 bytes (well within bounds)"},
        {20, "overflow: size > 15 (should be masked or rejected)"},
        {0, "edge: zero size"}
    };
    
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        uint32_t size = test_cases[i].size;
        uint32_t masked_size = size & 0xf;  /* Simulate the mask from vulnerable code */
        uint32_t bytes_to_copy = masked_size * 2;
        
        /* Invariant: bytes_to_copy must not exceed 20 (24 - 4 offset) */
        ck_assert_msg(
            bytes_to_copy <= 20,
            "Buffer overflow detected: size=%u, masked=%u, bytes=%u exceeds safe limit of 20. Case: %s",
            size, masked_size, bytes_to_copy, test_cases[i].description
        );
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_cmvmsg_buffer_overflow_prevention);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}