#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

#include "sgt_lib.h"
#include "../client/queue_lib.h"
#include "../client/stack_lib.h"
#include "../client/info_struct.h"

#define MAGIC_WORD "SGT\n"

int cmp(const char * const str_1, const char * const str_2) {
    return strcmp(str_1, str_2);
}

SGT *create_tree() {
    SGT *tree = calloc(1, sizeof(SGT));
    if (!tree) {
        return NULL;
    }
    
    tree->root = NULL;
    tree->alpha = 0.7;
    return tree;
}

static TreeNode *go_to_leaf(TreeNode * const node, const char * const key) {
    if (!node) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }
    
    TreeNode *current_parent = NULL;
    TreeNode *current = node;
    while (current) {
        current_parent = current;
        current = cmp(key, current->key) > 0 ? current->right : current->left;
    }

    return current_parent;

}

TreeNode *go_to_node(TreeNode * const node, const char * const key) {
    if (!node) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }
    
    TreeNode *current_parent = NULL;
    TreeNode *current = node;
    while (current) {
        current_parent = current;
        if (key && current->key && !cmp(key, current->key)) {
            break;
        }
        current = cmp(key, current->key) < 0 ? current->left : current->right;
    }
    
    if (current_parent != NULL && current_parent->key && key && cmp(current_parent->key, key) != 0) {
        return NULL;
    }
    return current_parent;
}

static TreeNode *create_tree_node(char * const key, Info * const info) {
    if (!key || !info || !info->info) {
        return NULL;
    }

    TreeNode *node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (!node) {
        return NULL;
    }
    
    node->size = 1;
    node->key = key;
    node->info = info;
    if (!node->info) {
        free(node);
        return NULL;
    }

    return node;
}

void free_info(TreeNode * const node) {
    if (!node) {
        return;
    }

    info_free(node->info);
    node->info = NULL;
    return;
}

void free_tree(SGT * const tree) {
    if (!tree) {
        return;
    }

    Queue *queue = queue_create();
    if (!queue) {
        free(tree);
        return;
    }

    if (queue_push(queue, tree->root) != QUEUE_OK) {
        queue_free(queue);
        free(tree);
        return;
    }

    while (queue_not_empty(queue)) {
        TreeNode *node = queue_pop(queue);

        if (node->left) {
            queue_push(queue, node->left);
        }
        if (node->right) {
            queue_push(queue, node->right);
        }
        
        free_info(node);
        free(node->key);
        free(node);
    }

    queue_free(queue);
    free(tree);
    return;
}

static void set_key_and_info(TreeNode * const node, char * const key, Info * const info) {
    if (!node || !key || !info) {
        return;
    }

    node->key = strdup(key);
    node->info = info;
    return;
}

TreeNode *find_release_tree(TreeNode * current, const char * const key, size_t * const current_release, const size_t need_release) {
    if (!current || !key) {
        return NULL;
    }

    (*current_release)++;
    if (!cmp(current->key, key) && *current_release == need_release) {
        return current;
    }
    
    TreeNode *found;
    found = find_release_tree(current->left, key, current_release, need_release);
    if (found) {
        return found;
    }
    found = find_release_tree(current->right, key, current_release, need_release);
    if (found) {
        return found;
    }

    return NULL;
}

TreeNode *find_tree(const SGT * const tree, const char * const key) {
    if (!tree || !tree->root) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }

    return go_to_node(tree->root, key);
}

TreeNode *special_find_tree(const SGT * const tree) {
    if (!tree || !tree->root) {
        return NULL;
    }

    TreeNode *ans = tree->root;
    TreeNode *current = tree->root;
    while (current) {
        if (ans != NULL && current != NULL && cmp(ans->key, current->key) > 0) {
            ans = current;
        }
        current = current->left;
    }

    return ans;
}

static int child_count(TreeNode * const current) {
    return (current->left != NULL) + (current->right != NULL);
}

static void update_size_after_delete(TreeNode * node) {
    if (!node) return;
    
    while (node) {
        node->size--;
        node = node->parent;
    }

    return;
}

static void child_reattachment(SGT * const tree, TreeNode * const current) {
    TreeNode *current_parent = current->parent;
    int count = child_count(current);
    TreeNode *child = NULL;
    TreeNode *child_parent = NULL;

    switch (count) {
        case 2: {
            child = current->right;
            child_parent = current;
            while (child->left) {
                child_parent = child;
                child = child->left;
            }
            char *min_key = strdup(child->key);
            Info *min_info = info_create(child->info->info);

            free_info(current);
            free(current->key);
            
            set_key_and_info(current, min_key, min_info);
            
            free(min_key);
                
            if (child_parent->left == child) {
                child_parent->left = child->right;
                if (child->right) {
                    child->right->parent = child_parent;
                }
            } else {
                child_parent->right = child->right;
                if (child->right) {
                    child->right->parent = child_parent;
                }
            }

            update_size_after_delete(child_parent);

            free_info(child);
            free(child->key);
            free(child);
            break;
        }
        case 1:
        case 0:
            free_info(current);
            free(current->key);
            child = (current->left) ? current->left : current->right;
            if (child) {
                child->parent = current_parent;
            }
            if (current_parent) {
                if (current_parent->left == current) {
                    current_parent->left = child;
                } else {
                    current_parent->right = child;
                }
            } else {
                tree->root = child;
            }

            update_size_after_delete(current_parent);

            free(current);
            break;
    }
    
    return;
}

static void collect_nodes(TreeNode * const node, TreeNode ** const arr, size_t * const idx) {
    if (!node || !arr || !idx) {
        return;
    }

    collect_nodes(node->left, arr, idx);
    arr[(*idx)++] = node;
    collect_nodes(node->right, arr, idx);

    return;
}

static TreeNode *build_balanced(TreeNode * const * const arr, size_t left, size_t right, TreeNode * const parent) {
    if (!arr || left > right) {
        return NULL;
    }

    size_t m = (left + right) / 2;
    TreeNode *root = arr[m];
    root->parent = parent;
    root->left = (m > left) ? build_balanced(arr, left, m - 1, root) : NULL;
    root->right = (m < right) ? build_balanced(arr, m + 1, right, root) : NULL;

    root->size = 1;

    root->size += root->left ? root->left->size : 0;
    root->size += root->right ? root->right->size : 0;

    return root;
}

static TreeNode *find_scapegoat(TreeNode * node, double alpha) {
    if (!node) {
        return NULL;
    }

    while (node->parent) {
        size_t size_left = node->left ? node->left->size : 0;
        size_t size_right = node->right ? node->right->size : 0;
        if (size_left > alpha * node->size || size_right > alpha * node->size) {
            return node->parent;
        }
        node = node->parent;
    }

    return NULL;
}

static void rebuild_subtree(SGT * const tree, TreeNode * const scapegoat) {
    if (!tree || !scapegoat) {
        return;
    }

    size_t n = scapegoat->size;
    TreeNode **arr = malloc(n * sizeof(TreeNode*));
    size_t idx = 0;
    collect_nodes(scapegoat, arr, &idx);
    TreeNode *parent = scapegoat->parent;
    TreeNode *new_sub = build_balanced(arr, 0, n-1, parent);
    if (!parent) {
        tree->root = new_sub;
    } else if (parent->left == scapegoat) {
        parent->left = new_sub;
    } else {
        parent->right = new_sub;
    }
    free(arr);

    return;
}

static size_t find_depth(TreeNode * node) {
    if (!node) {
        return 0;
    }

    size_t depth = 0;
    while (node->parent) {
        depth++;
        node = node->parent;
    }

    return depth;
}

static char is_balanced(SGT * const tree, const size_t depth) {
    if (!tree || tree->size <= 1) {
        return 0;
    }

    return depth <= (size_t)(log(1 / tree->alpha) / log(tree->size)) ? 1 : 0;
}

static void update_size_after_insert(TreeNode * node) {
    if (!node) {
        return;
    }

    while (node->parent) {
        node->parent->size++;
        node = node->parent;
    }

    return;
}

tree_err insert_tree(SGT * const tree, char * const key, Info * const info) {
    if (!tree) {
        return TREE_NULL;
    }
    if (!key || !info) {
        return TREE_VAL;
    }

    TreeNode *new_node = create_tree_node(key, info);
    if (!new_node) {
        return TREE_MEM;
    }

    TreeNode *current = go_to_leaf(tree->root, key);

    if (!current) {
        tree->root = new_node;
        tree->size = 1;
        tree->maxsize = 1;
        return TREE_OK;
    }

    if (strcmp(key, current->key) > 0) {
        current->right = new_node;
    } else {
        current->left = new_node;
    }
    new_node->parent = current;
    
    update_size_after_insert(new_node);

    tree->size++;

    if (!is_balanced(tree, find_depth(new_node))) {
        TreeNode *scapegoat = find_scapegoat(new_node->parent, tree->alpha);
        if (scapegoat) {
            rebuild_subtree(tree, scapegoat);
        }
    }

    if (tree->size > tree->maxsize) {
        tree->maxsize = tree->size;
    }

    return TREE_OK;
}

static void balancing_after_delete_tree(SGT * const tree) {
    if (!tree) {
        return;
    }

    if (tree->size <= tree->alpha * tree->maxsize) {
        if (tree->size > 0) {
            TreeNode **arr = malloc(tree->size * sizeof(TreeNode*));
            size_t idx = 0;
            collect_nodes(tree->root, arr, &idx);
            TreeNode *new_root = build_balanced(arr, 0, tree->size - 1, NULL);
            free(arr);
            tree->root = new_root;
        } else {
            tree->root = NULL;
        }
        tree->maxsize = tree->size;
    }

    return;
}
    
tree_err delete_tree(SGT * const tree, const char * const key) {
    if (!tree || !tree->root) {
        return TREE_NULL;
    }
    if (!key) {
        return TREE_VAL;
    }
    
    TreeNode *current = go_to_node(tree->root, key);
    if (!current) {
        return TREE_VAL;
    }
    child_reattachment(tree, current);

    tree->size--;

    balancing_after_delete_tree(tree);

    return TREE_OK;
}

tree_err traverse_tree(const SGT * const tree, const char * const min_key) {
    if (!tree || !tree->root) {
        return TREE_NULL;
    }

    Stack *stack = stack_create();
    if (!stack) {
        return TREE_VAL;
    }

    TreeNode *current = tree->root;
    while (current || stack_not_empty(stack)) {
        while (current) {
            stack_push(stack, current);
            current = current->right;
        }
        if (stack_not_empty(stack)) {
            current = stack_pop(stack);
            if (!min_key || strcmp(current->key, min_key) > 0) {
                printf("Key: %s, Info: ", current->key);
                info_print(current->info);
                printf("\n");
            }
            current = current->left;
        }
    }

    stack_free(stack);

    return TREE_OK;
}

static void add_nodes_edges(Agraph_t *g, TreeNode *node, Agnode_t *parent) {
    if (!node) {
        return;
    }

    char id_str[32];
    snprintf(id_str, sizeof(id_str), "%p", (void*)node);
    Agnode_t *n = agnode(g, id_str, 1);
    char key_str[32];
    snprintf(key_str, sizeof(key_str), "%s", node->key);
    agsafeset(n, "label", key_str, "");

    if (parent) {
        agedge(g, parent, n, NULL, 1);
    }

    // Левый ребенок
    if (node->left) {
        add_nodes_edges(g, node->left, n);
    } else {
        char fake_id[64];
        snprintf(fake_id, sizeof(fake_id), "%p_L", (void*)node);
        Agnode_t *fake = agnode(g, fake_id, 1);
        agsafeset(fake, "label", "0", "");
        agsafeset(fake, "style", "invis", "");
        Agedge_t *edge = agedge(g, n, fake, NULL, 1);
        agsafeset(edge, "style", "invis", "");
    }

    // Центральный ребенок (всегда фиктивный)
    char fake_id_c[64];
    snprintf(fake_id_c, sizeof(fake_id_c), "%p_C", (void*)node);
    Agnode_t *fake_c = agnode(g, fake_id_c, 1);
    agsafeset(fake_c, "label", "0", "");
    agsafeset(fake_c, "style", "invis", "");
    Agedge_t *edge_c = agedge(g, n, fake_c, NULL, 1);
    agsafeset(edge_c, "style", "invis", "");

    // Правый ребенок
    if (node->right) {
        add_nodes_edges(g, node->right, n);
    } else {
        char fake_id[64];
        snprintf(fake_id, sizeof(fake_id), "%p_R", (void*)node);
        Agnode_t *fake = agnode(g, fake_id, 1);
        agsafeset(fake, "label", "0", "");
        agsafeset(fake, "style", "invis", "");
        Agedge_t *edge = agedge(g, n, fake, NULL, 1);
        agsafeset(edge, "style", "invis", "");
    }

    return;
}

tree_err export_tree_svg(const SGT * const tree, const char * const filename) {
    if (!tree || !tree->root) {
        return TREE_EMPTY;
    }
    static unsigned long svg_export_counter = 0;
    svg_export_counter++;
    GVC_t *gvc = gvContext();
    char graph_name[64];
    snprintf(graph_name, sizeof(graph_name), "bst_%lu", svg_export_counter);
    Agraph_t *g = agopen(graph_name, Agdirected, NULL);
    add_nodes_edges(g, tree->root, NULL);
    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, "svg", filename);
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);

    return TREE_OK;
}

static void print_node(const TreeNode * const node, const char * const prefix, const char has_brother) {
    if (node == NULL) return;

    printf("%s", prefix);
    printf(has_brother ? "├── " : "└── ");
    printf("%s\n", node->key);
    
    size_t new_len_prefix = strlen(prefix) + 7;
    char *new_prefix = malloc(new_len_prefix);
    snprintf(new_prefix, new_len_prefix, "%s%s", prefix, has_brother ? "│   " : "    ");

    char right_has_brother = node->left != NULL;
    char left_has_brother = 0;

    if (node->right) {
        print_node(node->right, new_prefix, right_has_brother);
    }
    if (node->left) {
        print_node(node->left, new_prefix, left_has_brother);
    }

    free(new_prefix);
    return;
}

tree_err print_tree(const SGT *tree) {
    if (!tree || !tree->root) {
        return TREE_EMPTY;
    }
    
    print_node(tree->root, "", 0);
    return TREE_OK;
}

char *read_row_from_file(FILE * const file){
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
            res = realloc(res, (len_s + 1) * sizeof(char));
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
    }
    return res;
}

static tree_err check_magic_word(FILE * const file) {
    if (!file) {
        return FILE_ERR;
    }

    char magic_word[sizeof(MAGIC_WORD)] = {0};

    fgets(magic_word, sizeof(magic_word), file);
    if (cmp(magic_word, MAGIC_WORD) != 0) {
        return TREE_MAGIC_WORD;
    }

    return TREE_OK;
}

static tree_err read_node(SGT * const tree, FILE * const file) {
    if (!tree || !file) {
        return TREE_VAL;
    }
    
    tree_err result = TREE_OK;
    char *key = NULL;
    key = read_row_from_file(file);
    if (!key) {
        result = TREE_VAL;
    }
    
    char *str = NULL;
    str = read_row_from_file(file);
    if (!str) {
        result = TREE_VAL;
    }
    Info *info = info_create(str);
    if (!info) {
        result = TREE_VAL;
    }
        
    if (result == TREE_OK) {
        insert_tree(tree, key, info);
        free(str);
    }
    else {
        free(key);
        free(str);
        info_free(info);
    }
    
    return result;
}

tree_err import_tree(SGT * const tree, const char * const filename) {
    if (!tree) {
        return TREE_NULL;
    }
    if (!filename) {
        return TREE_VAL;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        return TREE_VAL;
    }
    
    tree_err result = TREE_OK;
    if (check_magic_word(file) != TREE_OK) {
        result = TREE_MAGIC_WORD;
        goto exit_with_err;
    }
    while (read_node(tree, file) == TREE_OK);
    result = TREE_VAL;

    goto exit_with_err;

exit_with_err:
    fclose(file);
    return result;
}

tree_err export_tree_txt(const SGT * const tree, const char * const filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        return FILE_ERR;
    }
    
    Stack *stack = stack_create();
    if (!stack) {
        fclose(file);
        return TREE_VAL;
    }

    fprintf(file, "%s", MAGIC_WORD);

    TreeNode *current = tree->root;
    while (current || stack_not_empty(stack)) {
        while (current) {
            stack_push(stack, current);
            current = current->left;
        }
        if (stack_not_empty(stack)) {
            current = stack_pop(stack);
            fprintf(file, "%s\n", current->key);
            info_print_file(current->info, file);
            fprintf(file, "\n");
            current = current->right;
        }
    }

    stack_free(stack);
    fclose(file);

    return TREE_OK;
}

