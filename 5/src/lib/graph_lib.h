#ifndef GRAPH_LIB_H
#define GRAPH_LIB_H

//TODO necessary files

typedef enum {
    ROOM_ENTRANCE,
    ROOM_EXIT, 
    ROOM_TRANSITION
} RoomType;

typedef struct Edge {
    char* dest_id;      // Идентификатор целевой вершины
    double length;      // Длина коридора
    struct Edge* next;  // Следующее ребро в списке
} Edge;

typedef struct Peak {
    char* id;           // Уникальный идентификатор комнаты
    RoomType type;      // Тип комнаты
    Edge* edges;        // Список исходящих рёбер
    struct Peak* next;// Следующая вершина в графе
} Peak;

typedef struct {
    Peak* vertices;   // Список вершин
    size_t vertex_count;// Количество вершин
} Graph;

Graph* graph_create(void);
Graph* graph_load_from_file(const char * const filename);

void graph_free(Graph * const graph);
void graph_print_adjacency_list(Graph * const graph);
void graph_save_to_file(Graph * const graph, const char * const filename);

bool graph_add_peak(Graph * const graph, const char * const id, RoomType type);
bool graph_remove_peak(Graph * const graph, const char * const id);
bool graph_add_edge(Graph * const graph, const char * const src_id, const char * const dest_id, double length);
bool graph_remove_edge(Graph * const graph, const char * const src_id, const char * const dest_id);
bool graph_update_vertex(Graph * const graph, const char * const id, RoomType new_type);
bool graph_update_edge(Graph * const graph, const char * const src_id, const char * const dest_id, double new_length);
bool dfs_find_exit(Graph * const graph, const char * const start_id);

typedef struct {
    char** path;        // Массив идентификаторов вершин
    size_t length;      // Количество вершин
    double total_dist;  // Общая длина пути
} Path;

Path* dijkstra_find_path(Graph * const graph, const char * const start_id, const char * const end_id);

void path_free(Path * path);
void graphviz_generate(Graph * const graph, const char *  const filename);
void graphviz_generate_with_path(Graph * const graph, Path * const path, const char * const filename);
void graphviz_generate_mst(Graph * const mst, const char * const filename);

Graph* mst_create(Graph * const graph);

Peak* graph_find_peak(Graph * const graph, const char * const id);

#endif
