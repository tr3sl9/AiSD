#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

#include "libgraph.h"
#include "hashtable.h"
#include "vertexarray.h"
#include "stack.h"
#include "priorityqueue.h"

#define MAGIC_WORD "MAZE\n"

static int cmp(const char * const str_1, const char * const str_2) {
    return strcmp(str_1, str_2);
}

Graph* graph_create(void) {
    Graph *graph = (Graph*)calloc(1, sizeof(Graph));
    if (!graph) {
        return NULL;
    }

    graph->vertices = create_table(10);
    if (!graph->vertices) {
        free(graph);
        return NULL;
    }

    return graph;
}

void delete_vertex_edges(Vertex * target) {
    if (!target) {
        return;
    }

    while (target->incoming_vertices) {
        Vertex *source = target->incoming_vertices->vertex;

        Edge *prev = NULL;
        Edge *current = source->edge;

        while (current) {
            if (current->dest == target) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    source->edge = current->next;
                }
                free(current);
                break;
            }

            prev = current;
            current = current->next;
        }

        IncomingVertex *next_incoming = target->incoming_vertices->next;
        free(target->incoming_vertices);
        target->incoming_vertices = next_incoming;
    }
    
    return;
}

void delete_incoming_vertices(Vertex * target) {
    if (!target) {
        return;
    }

    while (target->edge) {
        Vertex *neighbor = target->edge->dest;
        
        IncomingVertex *prev = NULL;
        IncomingVertex *current = neighbor->incoming_vertices;

        while (current) {
            if (current->vertex == target) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    neighbor->incoming_vertices = current->next;
                }
                free(current);
                break;
            }

            prev = current;
            current = current->next;
        }
        
        Edge *next_edge = target->edge->next;
        free(target->edge);
        target->edge = next_edge;
    }
    
    return;
}

static Vertex* vertex_create(const char * const id, RoomType type) {
    if (!id) {
        return NULL;
    }

    Vertex* vertex = (Vertex*)calloc(1, sizeof(Vertex));
    if (!vertex) {
        return NULL;
    }
    
    vertex->id = strdup(id);
    if (!vertex->id) {
        free(vertex);
        return NULL;
    }
    
    vertex->type = type;
    vertex->edge = NULL;
    vertex->incoming_vertices = NULL;
    
    return vertex;
}

Vertex* graph_find_vertex(Graph * const graph, const char * const id) {
    if (!graph || !id) {
        return NULL;
    }
    
    KeySpace* ks = search_vertex_in_table(graph->vertices, id);

    return ks ? ks->vertex : NULL;
}

graph_err graph_add_vertex(Graph * const graph, const char * const id, RoomType type) {
    if (!graph || !id) {
        return GRAPH_VAL;
    }
    
//    Vertex *existing = graph_find_vertex(graph, id);
//    if (existing) {
//        return GRAPH_ALREADY_EXISTS;
//    }
    
    Vertex *vertex = vertex_create(id, type);
    if (!vertex) {
        return GRAPH_MEM;
    }
    
    table_err result = insert_vertex_table(graph->vertices, vertex);
    if (result != TABLE_OK) {
        free(vertex->id);
        free(vertex);
        return (result == TABLE_VAL) ? GRAPH_ALREADY_EXISTS : GRAPH_MEM;
    }
    
    return GRAPH_OK;
}

graph_err graph_delete_vertex(Graph * const graph, const char * const id) {
    if (!graph || !id) {
        return GRAPH_VAL;
    }
    
    Vertex *target = graph_find_vertex(graph, id);
    if (!target) {
        return GRAPH_VAL;
    }

    delete_incoming_vertices(target);

    delete_vertex_edges(target);

    table_err result = delete_vertex_table(graph->vertices, id);

    return (result == TABLE_OK) ? GRAPH_OK : GRAPH_VAL;
}

void graph_free_vertices(Graph * const graph) {
    if (!graph) {
        return;
    }

    if (graph->vertices) {
        for (size_t i = 0; i < graph->vertices->msize; i++) {
            if (graph->vertices->ks[i].busy == BUSY && graph->vertices->ks[i].vertex) {
                graph_delete_vertex(graph, graph->vertices->ks[i].vertex->id);
            }
        }
        table_free(graph->vertices);
    }

    return;

}
void graph_free(Graph * const graph) {
    if (!graph) {
        return;
    }
    
    graph_free_vertices(graph);

    free(graph);

    return;
}

graph_err graph_update_vertex(Graph * const graph, const char * const id, RoomType new_type) {
    if (!graph || !id) {
        return GRAPH_VAL;
    }
    
    Vertex *vertex = graph_find_vertex(graph, id);
    if (!vertex) {
        return GRAPH_VAL;
    }
    
    vertex->type = new_type;
    return GRAPH_OK;
}

graph_err graph_add_edge(Graph * const graph, const char * const src_id, const char * const dest_id, size_t length) {
    if (!graph || !src_id || !dest_id) {
        return GRAPH_VAL;
    }
    
    Vertex *src = graph_find_vertex(graph, src_id);
    Vertex *dest = graph_find_vertex(graph, dest_id);
    
    if (!src || !dest) {
        return GRAPH_VAL;
    }
    
    Edge *new_edge = (Edge*)calloc(1, sizeof(Edge));
    if (!new_edge) {
        return GRAPH_MEM;
    }
    
    new_edge->dest = dest;
    new_edge->length = length;
    new_edge->next = src->edge;
    src->edge = new_edge;
    
    IncomingVertex *new_incoming = (IncomingVertex*)calloc(1, sizeof(IncomingVertex));
    if (!new_incoming) {
        src->edge = new_edge->next;
        free(new_edge);
        return GRAPH_MEM;
    }
    
    new_incoming->vertex = src;
    new_incoming->next = dest->incoming_vertices;
    dest->incoming_vertices = new_incoming;
    
    return GRAPH_OK;
}

static Edge* find_edge(Vertex *src, Vertex *dest) {
    if (!src || !dest) {
        return NULL;
    }
    
    Edge *current = src->edge;
    while (current) {
        if (current->dest == dest) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

static Edge* find_edge_by_length(Vertex *src, Vertex *dest, size_t length) {
    if (!src || !dest) {
        return NULL;
    }
    
    Edge *current = src->edge;
    while (current) {
        if (current->dest == dest && current->length == length) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

static void delete_edge_from_list(Vertex *src, Edge *edge_to_remove) {
    if (!src || !edge_to_remove) {
        return;
    }
    
    Edge *prev = NULL;
    Edge *current = src->edge;
    
    while (current) {
        if (current == edge_to_remove) {
            if (prev) {
                prev->next = current->next;
            } else {
                src->edge = current->next;
            }
            break;
        }
        prev = current;
        current = current->next;
    }

    return;
}

static void delete_incoming_vertex(Vertex *dest, Vertex *src) {
    if (!dest || !src) {
        return;
    }
    
    IncomingVertex *incoming_prev = NULL;
    IncomingVertex *incoming_current = dest->incoming_vertices;
    
    while (incoming_current) {
        if (incoming_current->vertex == src) {
            if (incoming_prev) {
                incoming_prev->next = incoming_current->next;
            } else {
                dest->incoming_vertices = incoming_current->next;
            }
            free(incoming_current);
            break;
        }
        incoming_prev = incoming_current;
        incoming_current = incoming_current->next;
    }

    return;
}

graph_err graph_delete_edge(Graph * const graph, const char * const src_id, const char * const dest_id) {
    if (!graph || !src_id || !dest_id) {
        return GRAPH_VAL;
    }
    
    Vertex *src = graph_find_vertex(graph, src_id);
    Vertex *dest = graph_find_vertex(graph, dest_id);
    
    if (!src || !dest) {
        return GRAPH_VAL;
    }
    
    Edge *edge = find_edge(src, dest);
    if (!edge) {
        return GRAPH_VAL;
    }
    
    delete_edge_from_list(src, edge);
    delete_incoming_vertex(dest, src);
    free(edge);
    
    return GRAPH_OK;
}

graph_err graph_update_edge(Graph * const graph, const char * const src_id, const char * const dest_id, size_t new_length) {
    if (!graph || !src_id || !dest_id) {
        return GRAPH_VAL;
    }
    
    Vertex *src = graph_find_vertex(graph, src_id);
    Vertex *dest = graph_find_vertex(graph, dest_id);
    
    if (!src || !dest) {
        return GRAPH_VAL;
    }
    
    Edge *edge = find_edge(src, dest);
    if (!edge) {
        return GRAPH_VAL;
    }
    
    edge->length = new_length;
    return GRAPH_OK;
}

graph_err graph_delete_edge_by_length(Graph * const graph, const char * const src_id, const char * const dest_id, size_t length) {
    if (!graph || !src_id || !dest_id) {
        return GRAPH_VAL;
    }
    
    Vertex *src = graph_find_vertex(graph, src_id);
    Vertex *dest = graph_find_vertex(graph, dest_id);
    
    if (!src || !dest) {
        return GRAPH_VAL;
    }
    
    Edge *edge = find_edge_by_length(src, dest, length);
    if (!edge) {
        return GRAPH_VAL;
    }
    
    delete_edge_from_list(src, edge);

    delete_incoming_vertex(dest, src);
    free(edge);
    
    return GRAPH_OK;
}

graph_err graph_update_edge_by_length(Graph * const graph, const char * const src_id, const char * const dest_id, size_t old_length, size_t new_length) {
    if (!graph || !src_id || !dest_id) {
        return GRAPH_VAL;
    }
    
    Vertex *src = graph_find_vertex(graph, src_id);
    Vertex *dest = graph_find_vertex(graph, dest_id);
    
    if (!src || !dest) {
        return GRAPH_VAL;
    }
    
    Edge *edge = find_edge_by_length(src, dest, old_length);
    if (!edge) {
        return GRAPH_VAL;
    }
    
    edge->length = new_length;
    return GRAPH_OK;
}

graph_err graph_get_edges_between(Graph * const graph, const char * const src_id, const char * const dest_id, Edge ***edges, size_t *count) {
    if (!graph || !src_id || !dest_id || !edges || !count) {
        return GRAPH_VAL;
    }
    
    Vertex *src = graph_find_vertex(graph, src_id);
    Vertex *dest = graph_find_vertex(graph, dest_id);
    
    if (!src || !dest) {
        return GRAPH_VAL;
    }
    
    *count = 0;
    *edges = NULL;
    
    Edge *current = src->edge;
    while (current) {
        if (current->dest == dest) {
            (*count)++;
        }
        current = current->next;
    }
    
    if (*count == 0) {
        return GRAPH_OK;
    }
    
    *edges = (Edge**)calloc(*count, sizeof(Edge*));
    if (!*edges) {
        return GRAPH_MEM;
    }
    
    size_t index = 0;
    current = src->edge;
    while (current && index < *count) {
        if (current->dest == dest) {
            (*edges)[index++] = current;
        }
        current = current->next;
    }
    
    return GRAPH_OK;
}

void graph_free_edges_list(Edge **edges) {
    if (edges) {
        free(edges);
    }

    return;
}

graph_err dfs_find_exit(Graph * const graph, const char * const start_id) {
    if (!graph || !start_id) {
        return GRAPH_VAL;
    }

    Vertex *start = graph_find_vertex(graph, start_id);
    if (!start || start->type != ROOM_ENTRANCE) {
        return GRAPH_VAL;
    }

    Stack *to_visit = stack_create();
    if (!to_visit) {
        return GRAPH_MEM;
    }

    VertexArray *visited = va_create();
    if (!visited) { 
        stack_free(to_visit); 
        return GRAPH_MEM; 
    }

    stack_push(to_visit, start);

    while (!stack_is_empty(to_visit)) {
        Vertex *vertex = (Vertex*)stack_pop(to_visit);
        if (va_find(visited, vertex)) {
            continue;
        }
        va_push(visited, vertex);

        if (vertex->type == ROOM_EXIT) {
            va_free(visited);
            stack_free(to_visit);
            return GRAPH_OK;
        }

        for (Edge *edge = vertex->edge; edge != NULL; edge = edge->next) {
            if (!va_find(visited, edge->dest)) {
                stack_push(to_visit, edge->dest);
            }
        }
    }

    va_free(visited);
    stack_free(to_visit);
    return GRAPH_VAL;
}

static Path* path_create(void) {
    return (Path*)calloc(1, sizeof(Path));
}

static long find_vertex_index_arr(Vertex * const * const vertices_arr, size_t vertex_count, Vertex * const target) {
    if (!vertices_arr || !target) {
        return -1;
    }
    
    for (size_t i = 0; i < vertex_count; i++) {
        if (vertices_arr[i] == target) {
            return (long)i;
        }
    }
    
    return -1;
}

Path* dijkstra_find_path(Graph * const graph, const char * const start_id, const char * const end_id) {
    if (!graph || !start_id || !end_id) {
        return NULL;
    }
    
    Vertex *start = graph_find_vertex(graph, start_id);
    if (!start || start->type != ROOM_ENTRANCE) {
        return NULL;
    }
    Vertex *end = graph_find_vertex(graph, end_id);
    if (!end || end->type != ROOM_EXIT) {
        return NULL;
    }

    size_t vertex_count = graph->vertices->csize;
    if (!vertex_count) {
        return NULL;
    }

    Vertex **vertices_arr = (Vertex**)calloc(vertex_count, sizeof(Vertex*));
    if (!vertices_arr) {
        return NULL;
    }

    size_t arr_index = 0;
    for (size_t i = 0; i < graph->vertices->msize && arr_index < vertex_count; i++) {
        if (graph->vertices->ks[i].busy == BUSY && graph->vertices->ks[i].vertex) {
            vertices_arr[arr_index++] = graph->vertices->ks[i].vertex;
        }
    }

    long start_index = find_vertex_index_arr(vertices_arr, vertex_count, start);
    long end_index = find_vertex_index_arr(vertices_arr, vertex_count, end);
    if (start_index < 0 || end_index < 0) { 
        free(vertices_arr); 
        return NULL; 
    }

    // Инициализация: d[v] = бесконечность, pred[v] = NULL для всех v
    size_t *d = (size_t*)calloc(vertex_count, sizeof(size_t));
    long *pred = (long*)calloc(vertex_count, sizeof(long));
    if (!d || !pred) { 
        free(vertices_arr); 
        free(d); 
        free(pred); 
        return NULL; 
    }

    for (size_t i = 0; i < vertex_count; i++) {
        d[i] = SIZE_MAX;
        pred[i] = -1;
    }

    d[start_index] = 0;

    PriorityQueue *pq = pq_create();
    if (!pq) { 
        free(vertices_arr); 
        free(d); 
        free(pred); 
        return NULL; 
    }

    // Добавляем все вершины в очередь с приоритетом d[v]
    for (size_t i = 0; i < vertex_count; i++) {
        if (!pq_insert(pq, vertices_arr[i], d[i])) {
            pq_free(pq);
            free(vertices_arr); 
            free(d); 
            free(pred); 
            return NULL;
        }
    }

    char *visited_vertices = (char*)calloc(vertex_count, sizeof(char));
    if (!visited_vertices) { 
        pq_free(pq); 
        free(vertices_arr); 
        free(d); 
        free(pred); 
        return NULL; 
    }

    while (!pq_is_empty(pq)) {

        Vertex *min = pq_extract_min(pq);
        if (!min) {
            continue;
        }
        
        long min_index = find_vertex_index_arr(vertices_arr, vertex_count, min);
        if (min_index < 0) {
            continue;
        }

        if (visited_vertices[min_index]) {
            continue;
        }

        visited_vertices[min_index] = 1;

        if (min_index == end_index) {
            break;
        }

        if (min->edge) {
            for (Edge *edge = min->edge; edge != NULL; edge = edge->next) {
                if (!edge || !edge->dest) {
                    continue;
                }
                
                Vertex *vertex = edge->dest;
                long vertex_index = find_vertex_index_arr(vertices_arr, vertex_count, vertex);
                if (vertex_index < 0) {
                    continue;
                }

                // Relax
                if (d[min_index] != SIZE_MAX && ((d[min_index] + edge->length) < d[vertex_index])) {
                    d[vertex_index] = d[min_index] + edge->length;
                    pred[vertex_index] = min_index;
                    
                    if (!visited_vertices[vertex_index]) {
                        pq_decrease_key(pq, vertex, d[vertex_index]);
                    }
                }
            }
        }
    }

    Path* path = NULL;
    
    if (d[end_index] == SIZE_MAX) {
        goto exit_with_free;
    }

    size_t path_len = 0;
    for (long vertex = end_index; vertex != -1; vertex = pred[vertex]) {
        path_len++;
    }

    path = path_create();
    if (!path) { 
        goto exit_with_free;
    }
    
    path->path = (char**)calloc(path_len, sizeof(char*));
    if (!path->path) { 
        free(path); 
        path = NULL;
        goto exit_with_free; 
    }

    path->length = path_len;

    size_t index = path_len;
    for (long vertex = end_index; vertex != -1; vertex = pred[vertex]) {
        path->path[--index] = strdup(vertices_arr[vertex]->id);
        if (!path->path[index]) {
            free(path); 
            path = NULL;
            goto exit_with_free;         
        }
    }

    path->total_dist = d[end_index];
    goto exit_with_free;

exit_with_free:
    pq_free(pq);
    free(visited_vertices);
    free(vertices_arr);
    free(d);
    free(pred);
    
    return path;
}

void path_free(Path * const path) {
    if (!path) {
        return;
    }

    if (path->path) {
        for (size_t i = 0; i < path->length; i++) {
            free(path->path[i]);
        }
        free(path->path);
    }
    
    free(path);

    return;
}

static void print_vertex(Vertex * const vertex) {
    if (!vertex) {
        return;
    }

    printf("Vertex '%s' (type: ", vertex->id);
    switch (vertex->type) {
        case ROOM_ENTRANCE: printf("ENTRANCE"); break;
        case ROOM_EXIT: printf("EXIT"); break;
        case ROOM_TRANSITION: printf("TRANSITION"); break;
    }
    printf("):\n");

    Edge *edge = vertex->edge;
    if (!edge) {
        printf("  No outgoing edges\n");
    } else {
        while (edge) {
            printf("  -> '%s' (length: %zu)\n", edge->dest->id, edge->length);
            edge = edge->next;
        }
    }
    printf("\n");

    return;
}

void graph_print_adjacency_list(Graph * const graph) {
    if (!graph || !graph->vertices) {
        return;
    }
    
    printf("Graph adjacency list:\n");
    printf("====================\n");
    
    for (size_t i = 0; i < graph->vertices->msize; i++) {
        if (graph->vertices->ks[i].busy == BUSY && graph->vertices->ks[i].vertex) {
            print_vertex(graph->vertices->ks[i].vertex);
        }
    }

    return;
}

static graph_err graphviz_add_vertices(Agraph_t *g, Graph * const graph) {
    if (!g || !graph) {
        return GRAPH_VAL;
    }

    for (size_t i = 0; i < graph->vertices->msize; i++) {
        if (graph->vertices->ks[i].busy == BUSY && graph->vertices->ks[i].vertex) {
            Vertex *vertex = graph->vertices->ks[i].vertex;
            
            Agnode_t *node = agnode(g, vertex->id, 1);
            if (node) {
                char *color = "blue";
                switch (vertex->type) {
                    case ROOM_ENTRANCE: color = "green"; break;
                    case ROOM_EXIT: color = "red"; break;
                    case ROOM_TRANSITION: color = ""; break;
                }
                
                agsafeset(node, "color", color, "");
                agsafeset(node, "shape", "circle", "");
                agsafeset(node, "style", "filled", "");
            }
        }
    }

    return GRAPH_OK;
}

static graph_err graphviz_add_edges(Agraph_t *g, Graph * const graph) {
    if (!g || !graph) {
        return GRAPH_VAL;
    }

    for (size_t i = 0; i < graph->vertices->msize; i++) {
        if (graph->vertices->ks[i].busy == BUSY && graph->vertices->ks[i].vertex) {
            Vertex *vertex = graph->vertices->ks[i].vertex;
            Edge *edge = vertex->edge;
            
            while (edge) {
                Agedge_t *e = agedge(g, agnode(g, vertex->id, 0), agnode(g, edge->dest->id, 0), NULL, 1);
                if (e) {
                    char length_str[32];
                    snprintf(length_str, sizeof(length_str), "%zu", edge->length);
                    agsafeset(e, "label", length_str, "");
                }
                edge = edge->next;
            }
        }
    }

    return GRAPH_OK;
}

graph_err graphviz_generate_svg(Graph * const graph, const char * const filename) {
    if (!graph || !filename) {
        return GRAPH_VAL;
    }

    Agraph_t *g = agopen("G", Agdirected, NULL);
    if (!g) {
        return GRAPH_MEM;
    }

    GVC_t *gvc = gvContext();
    if (!gvc) {
        agclose(g);
        return GRAPH_MEM;
    }

    graph_err result = graphviz_add_vertices(g, graph);
    if (result != GRAPH_OK) {
        gvFreeContext(gvc);
        agclose(g);
        return result;
    }

    result = graphviz_add_edges(g, graph);
    if (result != GRAPH_OK) {
        gvFreeContext(gvc);
        agclose(g);
        return result;
    }
    agsafeset(g, "rankdir", "LR", "");
    agsafeset(g, "node", "shape=circle, style=filled, fontname=Arial", "");

    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, "svg", filename);
    gvFreeLayout(gvc, g);
    gvFreeContext(gvc);

    agclose(g);

    return GRAPH_OK;
}

typedef struct UFNode {
    char* id;
    size_t parent;
    size_t rank;
} UFNode;

static size_t mst_find(UFNode * const uf_nodes, size_t x) {
    if (!uf_nodes) {
        return 0;
    } 

    if (uf_nodes[x].parent != x) {
        uf_nodes[x].parent = mst_find(uf_nodes, uf_nodes[x].parent);
    }

    return uf_nodes[x].parent;
}

static void mst_unite(UFNode * const uf_nodes, size_t x, size_t y) {
    if (!uf_nodes) {
        return;
    }

    size_t root_x = mst_find(uf_nodes, x);
    size_t root_y = mst_find(uf_nodes, y);

    if (root_x != root_y) {
        if (uf_nodes[root_x].rank < uf_nodes[root_y].rank) {
            uf_nodes[root_x].parent = root_y;
        } else if (uf_nodes[root_x].rank > uf_nodes[root_y].rank) {
            uf_nodes[root_y].parent = root_x;
        } else {
            uf_nodes[root_y].parent = root_x;
            uf_nodes[root_x].rank++;
        }
    }

    return;
}

static size_t mst_find_vertex_index(UFNode * const uf_nodes, size_t vertex_count, const char * const id) {
    if (!uf_nodes || !id) {
        return 0;
    }

    for (size_t i = 0; i < vertex_count; i++) {
        if (!cmp(uf_nodes[i].id, id)) {
            return i;
        }
    }

    return SIZE_MAX;
}

Graph* mst_create(Graph * const graph) {
    if (!graph) {
        return NULL;
    }

    size_t vertex_count = graph->vertices->csize;
    if (vertex_count == 0) {
        return NULL;
    }

    Graph *mst = graph_create();
    if (!mst) {
        return NULL;
    }

    typedef struct {
        char* src_id;
        char* dest_id;
        size_t length;
    } EdgeInfo;

    EdgeInfo *edges = (EdgeInfo*)calloc(10, sizeof(EdgeInfo));
    size_t edge_count = 0;
    size_t edge_capacity = 10;

    for (size_t i = 0; i < graph->vertices->msize; i++) {
        if (graph->vertices->ks[i].busy == BUSY && graph->vertices->ks[i].vertex) {
            Vertex *vertex = graph->vertices->ks[i].vertex;
            Edge *edge = vertex->edge;
            while (edge) {
                if (edge_count >= edge_capacity) {
                    edge_capacity = edge_capacity * 2;
                    edges = (EdgeInfo*)realloc(edges, edge_capacity * sizeof(EdgeInfo));
                }

                edges[edge_count].src_id = strdup(vertex->id);
                edges[edge_count].dest_id = strdup(edge->dest->id);
                edges[edge_count].length = edge->length;
                edge_count++;
                edge = edge->next;
            }
        }
    }

    // Сортируем рёбра по длине (алгоритм Крускала)
    for (size_t i = 0; i < edge_count - 1; i++) {
        for (size_t j = i + 1; j < edge_count; j++) {
            if (edges[i].length > edges[j].length) {
                EdgeInfo temp = edges[i];
                edges[i] = edges[j];
                edges[j] = temp;
            }
        }
    }

    for (size_t i = 0; i < graph->vertices->msize; i++) {
        if (graph->vertices->ks[i].busy == BUSY && graph->vertices->ks[i].vertex) {
            graph_add_vertex(mst, graph->vertices->ks[i].vertex->id, graph->vertices->ks[i].vertex->type);
        }
    }

    UFNode *uf_nodes = (UFNode*)calloc(vertex_count, sizeof(UFNode));
    if (!uf_nodes) {
        for (size_t j = 0; j < edge_count; j++) {
            free(edges[j].src_id);
            free(edges[j].dest_id);
        }
        free(edges);
        graph_free(mst);
        return NULL;
    }

    // Инициализируем Union-Find
    size_t uf_index = 0;
    for (size_t i = 0; i < graph->vertices->msize; i++) {
        if (graph->vertices->ks[i].busy == BUSY && graph->vertices->ks[i].vertex) {
            uf_nodes[uf_index].id = strdup(graph->vertices->ks[i].vertex->id);
            uf_nodes[uf_index].parent = uf_index;
            uf_nodes[uf_index].rank = 0;
            uf_index++;
        }
    }

    // добавляем рёбра в порядке возрастания длины
    size_t edges_added = 0;
    for (size_t i = 0; i < edge_count && edges_added < vertex_count - 1; i++) {
        size_t src_index = mst_find_vertex_index(uf_nodes, vertex_count, edges[i].src_id);
        size_t dest_index = mst_find_vertex_index(uf_nodes, vertex_count, edges[i].dest_id);
        
        if (src_index != SIZE_MAX && dest_index != SIZE_MAX) {
            if (mst_find(uf_nodes, src_index) != mst_find(uf_nodes, dest_index)) {
                graph_add_edge(mst, edges[i].src_id, edges[i].dest_id, edges[i].length);
                mst_unite(uf_nodes, src_index, dest_index);
                edges_added++;
            }
        }
    }

    for (size_t j = 0; j < edge_count; j++) {
        free(edges[j].src_id);
        free(edges[j].dest_id);
    }
    free(edges);

    for (size_t j = 0; j < vertex_count; j++) {
        free(uf_nodes[j].id);
    }
    free(uf_nodes);

    return mst;
}

static char *read_row_from_file(FILE * const file){
    if (!file) {
        return NULL;
    }
    
    char *res = NULL;
    char buf[81] = {0};
    int len = 0;
    int n = 0;
    
    do {
        n = fscanf(file, "%80[^\n]", buf);
        if(n < 0 && !res){
            return NULL;
        }
        else if(n > 0){
            int len_p = strlen(buf);
            int len_s = len + len_p;
            char *new_res = realloc(res, (len_s + 1) * sizeof(char));
            if (!new_res) {
                free(res);
                return NULL;
            }
            res = new_res;
            memcpy(res + len, buf, len_p);
            len = len_s;
        }
        else{
            fscanf(file, "%*c");
        }
    } while(n > 0);

    if(len > 0){
        res[len] = '\0';
    }
    else{
        res = calloc(1, sizeof(char));
        if (res) {
            res[0] = '\0';
        }
    }
    return res;
}

static graph_err check_magic_word(FILE * const file) {
    if (!file) {
        return GRAPH_FILE_ERR;
    }

    char magic_word[sizeof(MAGIC_WORD)] = {0};
    fgets(magic_word, sizeof(magic_word), file);
    
    if (strcmp(magic_word, MAGIC_WORD) != 0) {
        return GRAPH_MAGIC_WORD;
    }

    return GRAPH_OK;
}

static graph_err read_vertex(Graph * const graph, char * const line) {
    if (!line || !graph) {
        return GRAPH_VAL;
    }
    
    char id[128];
    int type;
    if (sscanf(line, "%127s %d", id, &type) == 2) {
        return graph_add_vertex(graph, id, (RoomType)type);
    }

    return GRAPH_VAL;
}

static graph_err read_edge(Graph * const graph, char * const line) {
    if (!line || !graph) {
        return GRAPH_VAL;
    }
    
    char src_id[128], dest_id[128];
    size_t length;
    if (sscanf(line, "%127s %127s %zu", src_id, dest_id, &length) == 3) {
        return graph_add_edge(graph, src_id, dest_id, length);
    }

    return GRAPH_VAL;
}

graph_err graph_import(Graph * const graph, const char * const filename) {
    if (!graph) {
        return GRAPH_NULL;
    }
    if (!filename) {
        printf("1");
        return GRAPH_VAL;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        return GRAPH_FILE_ERR;
    }
    
    graph_err result = check_magic_word(file);
    if (result != GRAPH_OK) {
        fclose(file);
        return result;
    }

    char *line = NULL;
    graph_err read_result = GRAPH_OK;
    result = GRAPH_OK;
    
    while ((line = read_row_from_file(file)) != NULL) {
        if (!strlen(line)) {
            free(line);
            continue; 
        }
        
        if (strncmp(line, "VERTEX ", 7) == 0) {
            read_result = read_vertex(graph, line + 7);
        }
        else if (strncmp(line, "EDGE ", 5) == 0) {
            read_result = read_edge(graph, line + 5);
        }
        else {
            read_result = GRAPH_OK;
        }
        
        free(line);
        
        if (read_result != GRAPH_OK && read_result != GRAPH_ALREADY_EXISTS) {
            result = read_result;
            break;
        }
    }
    
    fclose(file);
    return result;
}

static graph_err export_vertex(FILE * const file, const Vertex * const vertex) {
    if (!file || !vertex) {
        return GRAPH_VAL;
    }
    
    fprintf(file, "VERTEX %s %d\n", vertex->id, vertex->type);

    return GRAPH_OK;
}

static graph_err export_edge(FILE * const file, const Vertex * const src, const Edge * const edge) {
    if (!file || !src || !edge || !edge->dest){
        return GRAPH_VAL;
    }
    
    fprintf(file, "EDGE %s %s %zu\n", src->id, edge->dest->id, edge->length);

    return GRAPH_OK;
}

static graph_err export_all_vertices(FILE * const file, const Graph * const graph) {
    if (!file || !graph) {
        return GRAPH_VAL;
    }
    
    for (size_t i = 0; i < graph->vertices->msize; i++) {
        KeySpace *ks = &graph->vertices->ks[i];
        if (ks->busy == BUSY && ks->vertex) {
            graph_err result = export_vertex(file, ks->vertex);
            if (result != GRAPH_OK) {
                return result;
            }
        }
    }

    return GRAPH_OK;
}

static graph_err export_all_edges(FILE * const file, const Graph * const graph) {
    if (!file || !graph) {
        return GRAPH_VAL;
    }
    
    for (size_t i = 0; i < graph->vertices->msize; i++) {
        KeySpace *ks = &graph->vertices->ks[i];
        if (ks->busy == BUSY && ks->vertex) {
            Edge *edge = ks->vertex->edge;
            while (edge) {
                graph_err result = export_edge(file, ks->vertex, edge);
                if (result != GRAPH_OK) {
                    return result;
                }
                edge = edge->next;
            }
        }
    }

    return GRAPH_OK;
}

graph_err graph_export(const Graph * const graph, const char * const filename) {
    if (!graph) {
        return GRAPH_NULL;
    }
    if (!filename) {
        return GRAPH_VAL;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        return GRAPH_FILE_ERR;
    }

    fprintf(file, MAGIC_WORD);
    
    graph_err result = export_all_vertices(file, graph);
    if (result != GRAPH_OK) {
        fclose(file);
        return result;
    }
    
    result = export_all_edges(file, graph);
    if (result != GRAPH_OK) {
        fclose(file);
        return result;
    }
    
    fclose(file);
    return GRAPH_OK;
}


