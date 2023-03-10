#include "unit_tests.h"
#include <stdio.h>
#include "graphviz.h"
#include "history.h"


#define DATA_FOLDER "unit_tests/data/"
#define POS_IN(x) "pos_" x "_in.txt"
#define POS_OUT(x) "pos_" x "_out.txt"

#define TEST_HEAD(num)                                                                  \
                            int rc;                                                     \
                                                                                        \
                            graph_t *graph;                                             \
                            graph_t *exp_graph;                                         \
                            history_t *history;                                         \
                                                                                        \
                            graph = graph_from_file(DATA_FOLDER POS_IN( num ));         \
                            ck_assert_ptr_nonnull(graph);                               \
                            exp_graph = graph_from_file(DATA_FOLDER POS_OUT( num ));    \
                            ck_assert_ptr_nonnull(exp_graph);

#define TEST_TAIL                                                                       \
                            rc = compare_graphs(graph, exp_graph);                      \
                            ck_assert_int_eq(rc, GRAPHS_EQUAL);                         \
                                                                                        \
                            free_graph(graph);                                          \
                            free_graph(exp_graph);                                      \
                            free_history(history);

#define TEST(name, num)                                                                 \
                        START_TEST( name )                                              \
                        {                                                               \
                            TEST_HEAD( num )                                            \
                                                                                        \
                            history = get_lambda_transition_deletion_history(graph);    \
                            ck_assert_ptr_nonnull(history);                             \
                            do                                                          \
                            {                                                           \
                                rc = history_next(history, graph);                      \
                            }                                                           \
                            while (history->curr != history->prev);                     \
                                                                                        \
                            TEST_TAIL                                                   \
                        }                                                               \
                        END_TEST

#define TEST_DEBUG(name, num)                                                           \
                        START_TEST( name )                                              \
                        {                                                               \
                            TEST_HEAD( num )                                            \
                                                                                        \
                            puts("graph");                                              \
                            print_adjacency_list(graph);                                \
                            graph_visualize(graph, "temp");                             \
                                                                                        \
                            history = get_lambda_transition_deletion_history(graph);    \
                            ck_assert_ptr_nonnull(history);                             \
                            do                                                          \
                            {                                                           \
                                rc = history_next(history, graph);                      \
                                puts("new_graph");                                      \
                                print_adjacency_list(graph);                            \
                            }                                                           \
                            while (history->curr != history->prev);                     \
                                                                                        \
                            puts("exp_graph");                                          \
                            print_adjacency_list(exp_graph);                            \
                            graph_visualize(exp_graph, "temp_exp");                     \
                            puts("new_graph");                                          \
                            print_adjacency_list(graph);                                \
                            graph_visualize(graph, "temp_wo_lambda");                   \
                                                                                        \
                            TEST_TAIL                                                   \
                        }                                                               \
                        END_TEST

TEST(test_rm_lambda_3_verts_2_lambdas, "01")

TEST(test_rm_lambda_2_recursive_lambdas, "02")

TEST(test_rm_lambda_3_recursive_lambdas, "03")

TEST(test_rm_lambda_change_output, "04")

TEST(test_rm_lambda_node_lambda_loop, "05")

Suite *get_determinize_suite(void)
{
    Suite *s;
    TCase *tc_pos;

    s = suite_create("determinization");

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_rm_lambda_3_verts_2_lambdas);
    tcase_add_test(tc_pos, test_rm_lambda_2_recursive_lambdas);
    tcase_add_test(tc_pos, test_rm_lambda_3_recursive_lambdas);
    tcase_add_test(tc_pos, test_rm_lambda_change_output);
    tcase_add_test(tc_pos, test_rm_lambda_node_lambda_loop);
    suite_add_tcase(s, tc_pos);
    return s;
}
