#ifndef GRAPH_LIB_H
#define GRAPH_LIB_H

// Ошибки графа
typedef enum {
    GRAPH_OK = 0,
    GRAPH_MEM = 1,
    GRAPH_VAL = 2,
    GRAPH_ALREADY_EXISTS = 3,
    GRAPH_EMPTY = 4,
    GRAPH_NULL = 5,
    GRAPH_MAGIC_WORD = 6,
    GRAPH_ERR = 7,
    GRAPH_FILE_ERR = 8,
    GRAPH_EOF = -1,
    GRAPH_EXIT = -2
} graph_err;

typedef enum {
    ROOM_ENTRANCE = 0,
    ROOM_EXIT = 1, 
    ROOM_TRANSITION = 2
} RoomType;

typedef struct Edge {
    struct Vertex* dest;    // Куда ребро ведет
    size_t length;          // Длина коридора
    struct Edge* next;      // Следующее ребро в списке
} Edge;

typedef struct IncomingVertex {
    struct Vertex* vertex;
    struct IncomingVertex* next; 
} IncomingVertex;

typedef struct Vertex {
    char* id;               // Уникальный идентификатор комнаты
    RoomType type;          // Тип комнаты
    struct Edge* edge;     // Список исходящих рёбер
    struct IncomingVertex* incoming_vertices; // Список входящих вершин 
} Vertex;

typedef struct { 
    struct Table* vertices; // хэш-таблица вершин
} Graph;

Graph* graph_create(void);
Graph* graph_load_from_file(const char * const filename);

void graph_free(Graph * const graph);
void delete_incoming_vertices(Vertex * target);
void delete_vertex_edges(Vertex * target);
void free_incoming_vertices(IncomingVertex * incoming_vertices);
void graph_free_edges_list(Edge **edges);

graph_err graph_add_vertex(Graph * const graph, const char * const id, RoomType type);
graph_err graph_delete_vertex(Graph * const graph, const char * const id);
graph_err graph_update_vertex(Graph * const graph, const char * const id, RoomType new_type);
graph_err graph_add_edge(Graph * const graph, const char * const src_id, const char * const dest_id, size_t length);
graph_err graph_delete_edge(Graph * const graph, const char * const src_id, const char * const dest_id);
graph_err graph_delete_edge_by_length(Graph * const graph, const char * const src_id, const char * const dest_id, size_t length);
graph_err graph_update_edge(Graph * const graph, const char * const src_id, const char * const dest_id, size_t new_length);
graph_err graph_update_edge_by_length(Graph * const graph, const char * const src_id, const char * const dest_id, size_t old_length, size_t new_length);
graph_err graph_get_edges_between(Graph * const graph, const char * const src_id, const char * const dest_id, Edge ***edges, size_t *count);
graph_err dfs_find_exit(Graph * const graph, const char * const start_id);
graph_err graph_export(const Graph * const graph, const char * const filename);
graph_err graph_import(Graph * const graph, const char * const filename);
graph_err graphviz_generate_svg(Graph * const graph, const char * const filename);


typedef struct {
    char** path;        // Массив идентификаторов вершин
    size_t length;      // Количество вершин
    size_t total_dist;  // Общая длина пути
} Path;

Path* dijkstra_find_path(Graph * const graph, const char * const start_id, const char * const end_id);

void path_free(Path * path);
void graph_print_adjacency_list(Graph * const graph);
void graphviz_generate(Graph * const graph, const char * const filename);
//void graphviz_generate_with_path(Graph * const graph, Path * const path, const char * const filename);
//void graphviz_generate_mst(Graph * const mst, const char * const filename);

Graph* mst_create(Graph * const graph);

Vertex* graph_find_vertex(Graph * const graph, const char * const id);

#endif
