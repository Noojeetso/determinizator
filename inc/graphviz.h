#ifndef __GRAPHVIZ_H__
#define __GRAPHVIZ_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"
#include "presets.h"
#include "graph.h"

#define OUT_FOLDER "out/"
#define DOT_EXTENSION "dot"
#define IMAGE_EXTENSION "svg"

int
graph_visualize(graph_t *graph,
                char *graph_name);

int graph_visualize_highlited(graph_t *graph, graph_t *new_graph, char *graph_name);

#endif  // __GRAPHVIZ_H__
