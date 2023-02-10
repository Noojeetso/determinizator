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

typedef struct node node_t;
struct node
{
    int number;
    char value;
    node_t *next;
};

typedef struct adjacency_list
{
    node_t **vertices;
    size_t max_size;
} adjacecny_list_t;

struct graph
{
    adjacecny_list_t adjacency_list;
    operation_t *last_operation;
};

typedef struct graph graph_t;

graph_t *create_graph(size_t init_size);

int graph_expand(graph_t *graph, size_t new_size);

void free_graph(graph_t *graph);

graph_t *graph_copy(const graph_t *source_graph);

int graph_add_vertex(graph_t *graph, int number, vertex_type_t vertex_type);

int graph_add_arc(graph_t *graph, const int from_number, const int to_number, const char value);

int graph_contains_arc(const graph_t *graph, int from_number, int to_number, char value);

int update_vertex(graph_t *graph, history_t *history, int number, char value);

int update_arc(const graph_t *source_graph, graph_t *new_graph, history_t *history, int from_number, int to_number, char value);

bool is_output_vertex(node_t *vertex);

graph_t *graph_from_file(char *filename);

int write_graph_connections(FILE *file, const graph_t *graph, void *arg);

history_t *get_lambda_transition_deletion_history(const graph_t *source_graph);

int history_prev(history_t *history, graph_t *graph);

int history_next(history_t *history, graph_t *graph);

int apply_operation(graph_t *graph, operation_t *operation);

void print_adjacency_list(const graph_t *graph);

int compare_graphs(graph_t *graph_a, graph_t *graph_b);

#endif  // __GRAPHVIZ_H__
