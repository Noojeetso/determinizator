#include "unit_tests.h"
#include "graph.h"
#include <stdio.h>

#include "graphviz.h"

#define DATA_FOLDER "unit_tests/data/"
#define POS_IN(x) "pos_" x "_in.txt"
#define POS_OUT(x) "pos_" x "_out.txt"

START_TEST(test_rm_lambda_3verts_2lambdas)
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

    rc = graph_compare(new_graph, exp_graph);
    ck_assert_int_eq(rc, GRAPHS_EQUAL);

    free_graph(graph);
    free_graph(exp_graph);
    free_graph(new_graph);
}
END_TEST

START_TEST(test_rm_lambda_4verts_2lambdas_recursive)
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

    rc = graph_compare(new_graph, exp_graph);
    ck_assert_int_eq(rc, GRAPHS_EQUAL);

    free_graph(graph);
    free_graph(exp_graph);
    free_graph(new_graph);
}
END_TEST

START_TEST(test_rm_lambda_5verts_3lambdas_recursive)
{
    int rc;

    graph_t *graph;
    graph_t *new_graph;
    graph_t *exp_graph;

    graph = graph_from_file(DATA_FOLDER POS_IN("03"));
    ck_assert_ptr_nonnull(graph);

    exp_graph = graph_from_file(DATA_FOLDER POS_OUT("03"));
    ck_assert_ptr_nonnull(exp_graph);

    graph_visualize(graph, "temp");

    new_graph = remove_lambda_transitions(graph);

    graph_visualize(new_graph, "temp_wo_lambda");
    graph_visualize(exp_graph, "temp_exp");

    rc = graph_compare(new_graph, exp_graph);
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
    tcase_add_test(tc_pos, test_rm_lambda_3verts_2lambdas);
    tcase_add_test(tc_pos, test_rm_lambda_4verts_2lambdas_recursive);
    tcase_add_test(tc_pos, test_rm_lambda_5verts_3lambdas_recursive);
    suite_add_tcase(s, tc_pos);
    return s;
}
