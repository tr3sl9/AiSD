#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

#include "bst_lib.h"
#include "../client/info_struct.h"
#include "../client/queue_lib.h"
#include "../client/stack_lib.h"

#define MAGIC_WORD "BST\n"

static int cmp(const char * const str_1, const char * const str_2) {
    return strcmp(str_1, str_2);
}

BST *create_tree() {
    return (BST*)calloc(1, sizeof(BST));
}

static TreeNode *go_to_node(const BST * const tree, const size_t key) {
    if (!tree || !tree->root) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }
    
    TreeNode *current_parent = NULL;
    TreeNode *current = tree->root;
    while (current) {
        current_parent = current;
        if (key == current->key) {
            break;
        }
        current = key > current->key ? current->right : current->left;
    }

    return current_parent;
}

static TreeNode *go_to_leaf(const BST * const tree, const size_t key) {
    if (!tree || !tree->root) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }
    
    TreeNode *current_parent = NULL;
    TreeNode *current = tree->root;
    while (current) {
        current_parent = current;
        current = key > current->key ? current->right : current->left;
    }

    return current_parent;

}

static TreeNode *create_tree_node(const size_t key, Info * const info) {
    if (!info) {
        return NULL;
    }

    TreeNode *node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (!node) {
        return NULL;
    }

    node->key = key;
    node->info = info;
    if(!node->info) {
        free(node);
        return NULL;
    }

    return node;
}

void free_info(TreeNode * const node) {
    info_free(node->info);
    node->info = NULL;
    return;
}

void free_tree(BST * const tree) {
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
        free(node);
    }

    queue_free(queue);
    free(tree);
    return;
}

void set_key_and_info(TreeNode * const node, const size_t key, Info * const info) {
    if (!node || !key || !info) {
        return;
    }

    node->key = key;
    node->info = info;
    return;
}

TreeNode *special_find_tree(const BST * const tree, const size_t key) {
    if (!tree || !tree->root) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }

    TreeNode *ans = tree->root;
    TreeNode *current = tree->root;
    while (current) {
        if (key < current->key && current->key < ans->key) {
            ans = current;
        }
        current = key >= current->key ? current->right : current->left;
    }
    
    if (ans->key == key) {
        return NULL;
    }

    return ans;
}

TreeNode *find_tree(const BST * const tree, const size_t key) {
    if (!tree) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }

    return go_to_node(tree, key);
}

TreeNode *find_release_tree(TreeNode * current, const size_t key, const size_t release) {
    if (!current) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }

    size_t count = 1;
    while (current) {
        if (current->key == key && release == count) {
            return current;
        }
        else if (!current->left) {
            break;
        }
        else if (current->left->key == key) {
            count++;
            current = current->left;
        } else {
            break;
        }
    }

    return NULL;
}

tree_err insert_tree(BST * const tree, const size_t key, Info * const info) {
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

    TreeNode *current = go_to_leaf(tree, key);

    if (!current) {
        tree->root = new_node;
        return TREE_OK;
    }

    if (key > current->key) {
        current->right = new_node;
    } else {
        current->left = new_node;
    }
    new_node->parent = current;
    
    return TREE_OK;
}

static int child_count(TreeNode * const current) {
    return (current->left != NULL) + (current->right != NULL);
}

static void child_reattachment(BST * const tree, TreeNode * const current) {
    TreeNode *current_parent = current->parent;
    int count = child_count(current);
    TreeNode *child = NULL;
    TreeNode *child_parrent = NULL;
    switch (count) {
        case 2: {
            child = current->right;
            child_parrent = current;
            while (child->left) {
                child_parrent = child;
                child = child->left;
            }
            size_t min_key = child->key;
            Info *min_info = info_create(child->info->info);
            free_info(current);
            set_key_and_info(current, min_key, min_info);
            if (child_parrent->left == child) {
                child_parrent->left = child->right;
                if (child->right) {
                    child->right->parent = child_parrent;
                }
            } else {
                child_parrent->right = child->right;
                if (child->right) {
                    child->right->parent = child_parrent;
                }
            }
            free_info(child);
            free(child);
            break;
        }
        case 1:
        case 0:
            free_info(current);
            child = (current->left) ? current->left : current->right;
            if (child) child->parent = current_parent;
            if (current_parent) {
                if (current_parent->left == current) {
                    current_parent->left = child;
                } else {
                    current_parent->right = child;
                }
            } else {
                tree->root = child;
            }
            free(current);
            break;
    }
    
    return;
}

tree_err delete_tree(BST * const tree, const size_t key) {
    if (!tree) {
        return TREE_NULL;
    }
    if (!key) {
        return TREE_VAL;
    }

    TreeNode *current = go_to_node(tree, key);
    if (!current || current->key != key) {
        return TREE_VAL;
    }

    child_reattachment(tree, current);
        
    return TREE_OK;
}

tree_err traverse_tree(const BST * const tree) {
    if (!tree) {
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
            current = current->left;
        }
        if (stack_not_empty(stack)) {
            current = stack_pop(stack);
            printf("Key: %zu, Info: ", current->key);
            info_print(current->info);
            printf("\n");
            current = current->right;
        }
    }

    stack_free(stack);

    return TREE_OK;
}

static void add_nodes_edges(Agraph_t *g, TreeNode *node, Agnode_t *parent) {
    if (!node) {
        return;
    }

    char id_str[20];
    snprintf(id_str, sizeof(id_str), "%p", (void*)node);
    Agnode_t *n = agnode(g, id_str, 1);
    char key_str[20];
    snprintf(key_str, sizeof(key_str), "%zu", node->key);
    agsafeset(n, "label", key_str, "");

    if (parent) {
        agedge(g, parent, n, NULL, 1);
    }

    add_nodes_edges(g, node->left, n);
    add_nodes_edges(g, node->right, n);

    return;
}

tree_err export_tree_svg(const BST * const tree, const char * const filename) {
    if (!tree || !tree->root) {
        return TREE_EMPTY;
    }
/*
    char *ext = strrchr(filename, '.');
    char full_filename[256];

    if (ext && strcmp(ext, ".svg") == 0) {
        strncpy(full_filename, filename, sizeof(full_filename) - 1);
        full_filename[sizeof(full_filename) - 1] = '\0';
    } else {
        snprintf(full_filename, sizeof(full_filename), "%s.svg", filename);
    }
*/
    GVC_t *gvc = gvContext();
    Agraph_t *g = agopen("bst", Agdirected, NULL);
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
    printf("%zu\n", node->key);
    
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

tree_err print_tree(const BST *tree) {
    if (!tree || !tree->root) {
        return TREE_EMPTY;
    }
    
    print_node(tree->root, "", 0);
    return TREE_OK;
}

static tree_err check_magic_word(FILE * const file) {
    char magic_word[sizeof(MAGIC_WORD)] = {0};

    fgets(magic_word, sizeof(magic_word), file);
    if (cmp(magic_word, MAGIC_WORD) != 0) {
        return TREE_MAGIC_WORD;
    }

    return TREE_OK;
}

static tree_err read_node(BST * const tree, FILE * const file) {
    size_t key;
    Info *info = info_create(1);
    
    tree_err result = TREE_OK;
    if (fscanf(file, "%zu\n", &key) != 1) {
        result = TREE_VAL;
    }
    
    if (info_read(info, file) != 1) {
        result = TREE_VAL;
    }
    
    if (result == TREE_OK) {
        insert_tree(tree, key, info);
    }
    
    info_free(info);

    return result;
}

tree_err import_tree(BST * const tree, const char * const filename) {
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

tree_err export_tree_txt(const BST * const tree, const char * const filename) {
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
            fprintf(file, "%zu\n", current->key);
            info_print_file(current->info, file);
            fprintf(file, "\n");
            current = current->right;
        }
    }

    stack_free(stack);
    fclose(file);

    return TREE_OK;
}
