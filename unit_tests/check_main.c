#include "unit_tests.h"

int main(void)
{
    int failed_total = 0;

    SRunner *runner;

    Suite *determinize_suite = get_determinize_suite();

    runner = srunner_create(determinize_suite);
    srunner_set_fork_status(runner, CK_NOFORK);

    srunner_run_all(runner, CK_VERBOSE);
    failed_total = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (failed_total == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
