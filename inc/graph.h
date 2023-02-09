#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <queue.h>
#include <stdbool.h>
#include "stack.h"
#include "array.h"
#include "scan.h"
#include "presets.h"
#include "history.h"

#define GRAPH_EXPAND_FACTOR 1.5
#define INIT_GRAPH_SIZE 10

#define GRAPHS_EQUAL 1
#define GRAPHS_DIFFER 0

typedef enum vertex_type
{
    NO_VERTEX = 0,
    INTER_VERTEX = 1,
    INPUT_VERTEX = 2,
    OUTPUT_VERTEX = 4,
    VERTEX_MASK = 7
} vertex_type_t;

typedef struct graph graph_t;

void free_graph(graph_t *graph);

graph_t *graph_from_file(char *filename);

int write_graph_connections(FILE *file, const graph_t *graph, void *arg);

graph_t *remove_lambda_transitions(const graph_t *graph, history_t *history);

int history_prev(history_t *history, graph_t *graph);

int history_next(history_t *history, graph_t *graph);

void print_adjacency_list(graph_t *graph);

int compare_graphs(graph_t *graph_a, graph_t *graph_b);

#endif  // __GRAPHVIZ_H__
