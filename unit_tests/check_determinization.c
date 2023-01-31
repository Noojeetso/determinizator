#include "unit_tests.h"
#include "graph.h"
#include <stdio.h>

#include "graphviz.h"

#define DATA_FOLDER "unit_tests/data/"
#define POS_IN(x) "pos_" x "_in.txt"
#define POS_OUT(x) "pos_" x "_out.txt"

START_TEST(test_rm_lambda_3_verts_2_lambdas)
{
    int rc;

    graph_t *graph;
    graph_t *new_graph;
    graph_t *exp_graph;

    graph = graph_from_file(DATA_FOLDER POS_IN("01"));
    ck_assert_ptr_nonnull(graph);

    exp_graph = graph_from_file(DATA_FOLDER POS_OUT("01"));
    ck_assert_ptr_nonnull(exp_graph);

    new_graph = remove_lambda_transitions(graph);

    puts("exp_graph");
    print_adjacency_list(exp_graph);
    puts("new_graph");
    print_adjacency_list(new_graph);

    graph_visualize(graph, "temp");
    graph_visualize(new_graph, "temp_wo_lambda");
    graph_visualize(exp_graph, "temp_exp");

    rc = compare_graphs(new_graph, exp_graph);
    ck_assert_int_eq(rc, GRAPHS_EQUAL);

    free_graph(graph);
    free_graph(exp_graph);
    free_graph(new_graph);
}
END_TEST

START_TEST(test_rm_lambda_2_recursive_lambdas)
{
    int rc;

    graph_t *graph;
    graph_t *new_graph;
    graph_t *exp_graph;

    graph = graph_from_file(DATA_FOLDER POS_IN("02"));
    ck_assert_ptr_nonnull(graph);

    exp_graph = graph_from_file(DATA_FOLDER POS_OUT("02"));
    ck_assert_ptr_nonnull(exp_graph);

    new_graph = remove_lambda_transitions(graph);

    rc = compare_graphs(new_graph, exp_graph);
    ck_assert_int_eq(rc, GRAPHS_EQUAL);

    free_graph(graph);
    free_graph(exp_graph);
    free_graph(new_graph);
}
END_TEST

START_TEST(test_rm_lambda_3_recursive_lambdas)
{
    int rc;

    graph_t *graph;
    graph_t *new_graph;
    graph_t *exp_graph;

    graph = graph_from_file(DATA_FOLDER POS_IN("03"));
    ck_assert_ptr_nonnull(graph);

    exp_graph = graph_from_file(DATA_FOLDER POS_OUT("03"));
    ck_assert_ptr_nonnull(exp_graph);

    new_graph = remove_lambda_transitions(graph);

    rc = compare_graphs(new_graph, exp_graph);
    ck_assert_int_eq(rc, GRAPHS_EQUAL);

    free_graph(graph);
    free_graph(exp_graph);
    free_graph(new_graph);
}
END_TEST

START_TEST(test_rm_lambda_change_output)
{
    int rc;

    graph_t *graph;
    graph_t *new_graph;
    graph_t *exp_graph;

    graph = graph_from_file(DATA_FOLDER POS_IN("04"));
    ck_assert_ptr_nonnull(graph);

    exp_graph = graph_from_file(DATA_FOLDER POS_OUT("04"));
    ck_assert_ptr_nonnull(exp_graph);

    new_graph = remove_lambda_transitions(graph);

    rc = compare_graphs(new_graph, exp_graph);
    ck_assert_int_eq(rc, GRAPHS_EQUAL);

    free_graph(graph);
    free_graph(exp_graph);
    free_graph(new_graph);
}
END_TEST

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
    suite_add_tcase(s, tc_pos);
    return s;
}
