#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

#include "../lib/libgraph.h"
#include "dialog.h"
#include "readnumber.h"

#define PROMPT_FOR_VERTEX "Enter vertex ID: "
#define PROMPT_FOR_ROOM_TYPE "Enter room type (0=entrance, 1=exit, 2=transition): "
#define PROMPT_FOR_SRC_VERTEX "Enter source vertex ID: "
#define PROMPT_FOR_DEST_VERTEX "Enter destination vertex ID: "
#define PROMPT_FOR_LENGTH "Enter corridor length: "
#define PROMPT_FOR_FILE "Enter filename: "
#define PROMPT_FOR_START_VERTEX "Enter start vertex ID: "
#define PROMPT_FOR_END_VERTEX "Enter end vertex ID: "
#define PROMPT_FOR_EDGE_LENGTH "Enter edge length to select: "
#define PROMPT_FOR_NEW_LENGTH "Enter new length: "

static void display_edges_between(Graph * const graph, const char * const src_id, const char * const dest_id) {
    if (!graph || !src_id || !dest_id) {
        return;
    }
    
    Edge **edges = NULL;
    size_t count = 0;
    
    graph_err result = graph_get_edges_between(graph, src_id, dest_id, &edges, &count);
    if (result != GRAPH_OK || count == 0) {
        printf("No edges found between '%s' and '%s'.\n", src_id, dest_id);
        return;
    }
    
    printf("Edges between '%s' and '%s':\n", src_id, dest_id);
    for (size_t i = 0; i < count; i++) {
        printf("  %zu. Length: %zu\n", i + 1, edges[i]->length);
    }
    
    graph_free_edges_list(edges);

    return;
}

static graph_err dialog_add_vertex(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *id = readline(PROMPT_FOR_VERTEX);
    if (!id) {
        return GRAPH_EOF;
    }
    
    graph_err result = GRAPH_OK;
    int room_type = -1;
    read_number(&room_type, -1, 2, PROMPT_FOR_ROOM_TYPE);
    if (room_type == -1) {
        result = GRAPH_VAL;
        goto exit_with_err;
    }

    result = graph_add_vertex(graph, id, (RoomType)room_type);
    if (result != GRAPH_OK) {
        goto exit_with_err;
    }

    printf("Vertex '%s' added successfully.\n", id);

exit_with_err:
    free(id);
    return result;
}

static graph_err dialog_add_edge(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *src_id = readline(PROMPT_FOR_SRC_VERTEX);
    if (!src_id) {
        return GRAPH_EOF;
    }

    char *dest_id = readline(PROMPT_FOR_DEST_VERTEX);
    if (!dest_id) {
        free(src_id);
        return GRAPH_EOF;
    }

    size_t length = 1;
    read_positive_number(&length, PROMPT_FOR_LENGTH);
    if (length < 1) {
        free(src_id);
        free(dest_id);
        return GRAPH_VAL;
    }

    graph_err result = graph_add_edge(graph, src_id, dest_id, length);
    if (result == GRAPH_OK) {
        printf("Edge from '%s' to '%s' with length %zu added successfully.\n", src_id, dest_id, length);
    }

    free(src_id);
    free(dest_id);
    return result;
}

static graph_err dialog_delete_vertex(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *id = readline(PROMPT_FOR_VERTEX);
    if (!id) {
        return GRAPH_EOF;
    }

    graph_err result = graph_delete_vertex(graph, id);
    if (result == GRAPH_OK) {
        printf("Vertex '%s' removed successfully.\n", id);
    }

    free(id);
    return result;
}

static graph_err dialog_delete_edge(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *src_id = readline(PROMPT_FOR_SRC_VERTEX);
    if (!src_id) {
        return GRAPH_EOF;
    }

    char *dest_id = readline(PROMPT_FOR_DEST_VERTEX);
    if (!dest_id) {
        free(src_id);
        return GRAPH_EOF;
    }

    display_edges_between(graph, src_id, dest_id);
    
    Edge **edges = NULL;
    size_t count = 0;
    graph_err check_result = graph_get_edges_between(graph, src_id, dest_id, &edges, &count);
    
    if (check_result != GRAPH_OK || count == 0) {
        printf("No edges found between '%s' and '%s'.\n", src_id, dest_id);
        graph_free_edges_list(edges);
        free(src_id);
        free(dest_id);
        return GRAPH_VAL;
    }
    
    graph_free_edges_list(edges);
    
    if (count == 1) {
        graph_err result = graph_delete_edge(graph, src_id, dest_id);
        if (result == GRAPH_OK) {
            printf("Edge from '%s' to '%s' removed successfully.\n", src_id, dest_id);
        }
        free(src_id);
        free(dest_id);
        return result;
    }
    
    size_t length = 0;
    read_positive_number(&length, PROMPT_FOR_EDGE_LENGTH);
    if (length < 1) {
        free(src_id);
        free(dest_id);
        return GRAPH_VAL;
    }
    
    graph_err result = graph_delete_edge_by_length(graph, src_id, dest_id, length);
    if (result == GRAPH_OK) {
        printf("Edge from '%s' to '%s' with length %zu removed successfully.\n", src_id, dest_id, length);
    } else {
        printf("No edge with length %zu found between '%s' and '%s'.\n", length, src_id, dest_id);
    }

    free(src_id);
    free(dest_id);
    return result;
}

static graph_err dialog_update_vertex(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *id = readline(PROMPT_FOR_VERTEX);
    if (!id) {
        return GRAPH_EOF;
    }
    
    int room_type = -1;
    read_number(&room_type, -1, 3, PROMPT_FOR_ROOM_TYPE);
    if (room_type == -1) {
        free(id);
        return GRAPH_VAL;
    }

    graph_err result = graph_update_vertex(graph, id, (RoomType)room_type);
    free(id);
    return result;
}

static graph_err dialog_update_edge(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *src_id = readline(PROMPT_FOR_SRC_VERTEX);
    if (!src_id) {
        return GRAPH_EOF;
    }

    char *dest_id = readline(PROMPT_FOR_DEST_VERTEX);
    if (!dest_id) {
        free(src_id);
        return GRAPH_EOF;
    }

    display_edges_between(graph, src_id, dest_id);
    
    Edge **edges = NULL;
    size_t count = 0;
    graph_err check_result = graph_get_edges_between(graph, src_id, dest_id, &edges, &count);
    
    if (check_result != GRAPH_OK || count == 0) {
        printf("No edges found between '%s' and '%s'.\n", src_id, dest_id);
        graph_free_edges_list(edges);
        free(src_id);
        free(dest_id);
        return GRAPH_VAL;
    }
    
    graph_free_edges_list(edges);
    
    if (count == 1) {
        size_t new_length = 0;
        graph_err rn = read_positive_number(&new_length, PROMPT_FOR_NEW_LENGTH);
        if (rn != GRAPH_OK || new_length < 1) { 
            free(src_id); free(dest_id); 
            return rn != GRAPH_OK ? rn : GRAPH_VAL; 
        }

        graph_err result = graph_update_edge(graph, src_id, dest_id, new_length);
        if (result == GRAPH_OK) {
            printf("Edge from '%s' to '%s' updated with length %zu.\n", src_id, dest_id, new_length);
        }
        free(src_id);
        free(dest_id);
        return result;
    }
    
    size_t old_length = 0;
    read_positive_number(&old_length, PROMPT_FOR_EDGE_LENGTH);
    if (old_length < 1) {
        free(src_id);
        free(dest_id);
        return GRAPH_VAL;
    }
    
    size_t new_length = 0;
    graph_err rn = read_positive_number(&new_length, PROMPT_FOR_NEW_LENGTH);
    if (rn != GRAPH_OK || new_length < 1) { 
        free(src_id); free(dest_id); 
        return rn != GRAPH_OK ? rn : GRAPH_VAL; 
    }
    
    graph_err result = graph_update_edge_by_length(graph, src_id, dest_id, old_length, new_length);
    if (result == GRAPH_OK) {
        printf("Edge from '%s' to '%s' with length %zu updated to length %zu.\n", src_id, dest_id, old_length, new_length);
    } else {
        printf("No edge with length %zu found between '%s' and '%s'.\n", old_length, src_id, dest_id);
    }

    free(src_id);
    free(dest_id);
    return result;
}

static graph_err dialog_find_exit(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *start_id = readline(PROMPT_FOR_START_VERTEX);
    if (!start_id) {
        return GRAPH_EOF;
    }

    graph_err result = dfs_find_exit(graph, start_id);
    if (result == GRAPH_OK) {
        printf("Exit found from vertex '%s'!\n", start_id);
    } else {
        printf("No exit found from vertex '%s'.\n", start_id);
    }

    free(start_id);
    return result;
}

static graph_err dialog_find_shortest_path(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *start_id = readline(PROMPT_FOR_START_VERTEX);
    if (!start_id) {
        return GRAPH_EOF;
    }

    char *end_id = readline(PROMPT_FOR_END_VERTEX);
    if (!end_id) {
        free(start_id);
        return GRAPH_EOF;
    }

    Path* path = dijkstra_find_path(graph, start_id, end_id);
    if (path) {
        printf("Shortest path from '%s' to '%s':\n", start_id, end_id);
        for (size_t i = 0; i < path->length; i++) {
            printf("%s", path->path[i]);
            if (i < path->length - 1) {
                printf(" -> ");
            }
        }
        printf("\nTotal distance: %zu\n", path->total_dist);
        path_free(path);
    } else {
        printf("No path found from '%s' to '%s'.\n", start_id, end_id);
    }

    free(start_id);
    free(end_id);

    return path ? GRAPH_OK : GRAPH_VAL;
}

static graph_err dialog_create_mst(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    Graph* mst = mst_create(graph);
    if (!mst) {
        printf("Failed to create minimum spanning tree.\n");
        return GRAPH_MEM;
    }

    graph_free(graph);
    graph->vertices = mst->vertices; 
    free(mst);
    
    printf("Graph modified to minimum spanning tree successfully.\n");
    return GRAPH_OK;
}

static graph_err dialog_print_graph(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    graph_print_adjacency_list(graph);
    return GRAPH_OK;
}

static graph_err dialog_export_graphviz(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *filename = readline(PROMPT_FOR_FILE);
    if (!filename) {
        return GRAPH_EOF;
    }

    graph_err result = graphviz_generate_svg(graph, filename);

    free(filename);
    return result;
}

static graph_err dialog_load_from_file(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *filename = readline(PROMPT_FOR_FILE);
    if (!filename) {
        return GRAPH_EOF;
    }

    graph_err err = graph_import(graph, filename);

    free(filename);
    return err;
}

static graph_err dialog_save_to_file(Graph * const graph) {
    if (!graph) {
        return GRAPH_NULL;
    }

    char *filename = readline(PROMPT_FOR_FILE);
    if (!filename) {
        return GRAPH_EOF;
    }
    
    graph_err err = graph_export(graph, filename);

    free(filename);
    return err;
}

static graph_err dialog_exit(__attribute__((__unused__)) Graph * const graph) {
    printf("Goodbye!\n");
    return GRAPH_EXIT;
}

static const operation operations[] = {
    {dialog_add_vertex, "Add vertex"},
    {dialog_add_edge, "Add edge"},
    {dialog_delete_vertex, "Remove vertex"},
    {dialog_delete_edge, "Remove edge"},
    {dialog_update_vertex, "Update vertex"},
    {dialog_update_edge, "Update edge"},
    {dialog_find_exit, "Find exit (DFS)"},
    {dialog_find_shortest_path, "Find shortest path (Dijkstra)"},
    {dialog_create_mst, "Create minimum spanning tree"},
    {dialog_print_graph, "Print graph"},
    {dialog_export_graphviz, "Export to Graphviz"},
    {dialog_load_from_file, "Load from file"},
    {dialog_save_to_file, "Save to file"},
    {dialog_exit, "Exit"}
};

#define COUNT_OP (sizeof(operations) / sizeof(operation))

size_t get_operation_count(void) {
    return COUNT_OP;
}

void show_menu(void) {
    printf("\n=== Maze Graph Operations ===\n");
    for (size_t i = 0; i < COUNT_OP; i++) {
        printf("%zu. %s\n", i + 1, operations[i].msg);
    }

    return;
}

static void choice_msg_from_graph_err(const graph_err err) {
    switch (err) {
        case GRAPH_OK: printf("ALL'S OKAY\n"); break;
        case GRAPH_EMPTY: printf("Error: Graph is empty\n"); break;
        case GRAPH_MEM: printf("Error: Memory allocation failed\n"); break;
        case GRAPH_NULL: printf("Error: Graph is null\n"); break;
        case GRAPH_VAL: printf("Error: value\n"); break;
        case GRAPH_MAGIC_WORD: printf("Error: Invalid file format\n"); break;
        case GRAPH_FILE_ERR: printf("Error: Cannot open file\n"); break;
        case GRAPH_EOF: printf("Error: EOF\n"); break;                       
        case GRAPH_EXIT: printf("EXIT\n"); break;
        default: printf("Unknown error\n");    
    }

    return;
}

int process_choice(Graph* const graph, size_t choice) {
    if (choice < 1 || choice > COUNT_OP) {
        printf("Invalid choice. Please enter a number between 1 and %zu.\n", COUNT_OP);
        return 0;
    }

    graph_err result = operations[choice - 1].func(graph);
    choice_msg_from_graph_err(result);
    
    return (result == GRAPH_EOF || result == GRAPH_EXIT) ? 1 : 0;
}
