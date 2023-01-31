#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <queue.h>
#include "stack.h"
#include "array.h"
#include "scan.h"
#include "presets.h"

#define GRAPH_EXPAND_FACTOR 1.5
#define INIT_GRAPH_SIZE 10

#define GRAPHS_EQUAL 1
#define GRAPHS_DIFFER 0

typedef struct graph graph_t;

void free_graph(graph_t *graph);

graph_t *graph_from_file(char *filename);

// graph_t *graph_from_matrix(char (*matrix)[], size_t size);

int write_graph_connections(FILE *file, graph_t *graph, void *arg);

graph_t *remove_lambda_transitions(graph_t *graph);  // , history_t *history)

int graph_compare(graph_t *graph_a, graph_t *graph_b);

#endif  // __GRAPHVIZ_H__
