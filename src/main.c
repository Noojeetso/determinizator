#include <stdlib.h>
// #include <stdio.h>
#include "graphviz.h"

int main(void)
{
    int rc;
    graph_t *graph;
    graph_t *new_graph;

    graph = graph_from_file("graph_in.txt");
    if (graph == NULL)
        return EXIT_FAILURE;

    rc = graph_visualize(graph, "graph");

    new_graph = remove_lambda_transitions(graph);

    // rc = graph_visualize_highlited(graph, new_graph, "graph_wo_lambda");
    rc = graph_visualize(new_graph, "graph_wo_lambda");

    free_graph(graph);
    free_graph(new_graph);

    return rc;
}
