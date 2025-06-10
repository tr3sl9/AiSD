#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include "../lib/bst_lib.h"
#include "dialog.h"
#include "testing.h"

#define PROMPT_FOR_KEY "Enter key: "
#define PROMPT_FOR_INFO "Enter info: "
#define PROMPT_FOR_FILE "Enter filename: "
#define PROMPT_FOR_RELEASE "Enter release: "
#define COUNT_OP sizeof(operations) / sizeof(operation)

tree_err dialog_insert(BST * const tree) {
    size_t key, info;
    if (read_positive_number(&key, PROMPT_FOR_KEY) == TREE_EOF) {
        return TREE_EOF;
    }
    if (read_positive_number(&info, PROMPT_FOR_INFO) == TREE_EOF) {
        return TREE_EOF;
    }

    Info *info_ptr = info_create();
    if (!info_ptr || !info_insert(info_ptr, info)) {
        info_free(info_ptr);
        return TREE_MEM;
    }
    return insert_tree(tree, key, info_ptr);
}

tree_err dialog_delete(BST * const tree) {
    if (!tree->root) {
        return TREE_EMPTY;
    }

    size_t key;
    if (read_positive_number(&key, PROMPT_FOR_KEY) == TREE_EOF) {
        return TREE_EOF;
    }

    return delete_tree(tree, key);
}

static tree_err search_key_release(TreeNode * const node, size_t * const release) { 
    if ((node->left && node->key == node->left->key)) {
        printf("The element with this key is not the only one. Specify release");
        
        if (read_positive_number(release, PROMPT_FOR_RELEASE) == TREE_EOF) {
            return TREE_EOF;
        }

        TreeNode *release_node = find_release_tree(node, node->key, *release);
        if (!release_node) {
            return TREE_VAL;
        }

    }
    
    return TREE_OK;
}

tree_err dialog_find(BST * const tree) {
    if (!tree) {
        return TREE_NULL;
    }
    if (!tree->root) {
        return TREE_EMPTY;
}

    size_t key;
    if (read_positive_number(&key, PROMPT_FOR_KEY) == TREE_EOF) {
        return TREE_EOF;
    }

    TreeNode *node = find_tree(tree, key);
    if (!node) {
        return TREE_VAL;
    }

    size_t release = 1;
    tree_err error = search_key_release(node, &release);
    if (error != TREE_OK) {
        return error;
    }

    printf("\nFound element with key %zu, release %zu:\n", key, release);
    printf("┌──────────────────────┐\n");
    printf("│ Info                 │\n");
    printf("├──────────────────────┤\n");
    printf("│ %-20zu │\n", node->info->info);
    printf("└──────────────────────┘\n");

    return TREE_OK;
}

tree_err dialog_special_find(BST * const tree) {
    if (!tree) {
        return TREE_NULL;
    }
    if (!tree->root) {
        return TREE_EMPTY;
    }

    size_t key;
    if (read_positive_number(&key, PROMPT_FOR_KEY) == TREE_EOF) {
        return TREE_EOF;
    }

    TreeNode *node = special_find_tree(tree, key);
    if (!node) {
        return TREE_VAL;
    }
    
    size_t release = 1;
    tree_err error = search_key_release(node, &release);
    if (error != TREE_OK) {
        return error;
    }

    printf("\nFound element with key %zu, release %zu:\n", node->key, release);
    printf("┌──────────────────────┐\n");
    printf("│ Info                 │\n");
    printf("├──────────────────────┤\n");
    printf("│ %-20zu │\n", node->info->info);
    printf("└──────────────────────┘\n");

    return TREE_OK;
}

tree_err dialog_walk(BST * const tree) {
    return traverse_tree(tree);
}

tree_err dialog_export(BST * const tree) {
    char *filename = readline(PROMPT_FOR_FILE);
    if (!filename) {
        free(filename);
        return TREE_EOF;
    }

    tree_err err = bst_export(tree, filename);
    free(filename);
    return err;
}

static tree_err dialog_exit(__attribute__((__unused__)) BST * const tree) {
    return TREE_EXIT;
}

const operation operations[] = {
    {dialog_insert, "Insert element"},
    {dialog_delete, "Delete by key"},
    {dialog_find, "Search by key"},
    {dialog_special_find, "Special search"},
    {dialog_walk, "Print tree"},
    {dialog_export, "Export tree to file"},
    {dialog_exit, "EXIT"}
};

void show_menu() {
    printf("\nMenu:\n");
    for (size_t i = 0; i < COUNT_OP; i++) {
        printf("[%zu]: %s\n", i + 1, operations[i].msg);
    }
    printf("\n");
}

static void choice_msg_from_tree_err(const tree_err err) {
    switch (err) {
        case TREE_OK: printf("Operation completed successfully\n"); break;
        case TREE_EMPTY: printf("Error: Tree is empty\n"); break;
        case TREE_NULL: printf("Error: Tree is null\n"); break;
        case TREE_MEM: printf("Error: Memory allocation failed\n"); break;
        case TREE_VAL: printf("Error: Invalid value\n"); break;
        case TREE_EOF: printf("Error: EOF\n"); break;
        case TREE_EXIT: printf("EXIT\n"); break;
        default: printf("Unknown error\n");
    }
}

int process_choice(BST *tree, size_t choice) {
    if (choice < 1 || choice > COUNT_OP) {
        printf("Invalid choice\n");
        return 0;
    }
    tree_err err = operations[choice - 1].func(tree);
    choice_msg_from_tree_err(err);
    return err == TREE_EOF || err == TREE_EXIT;
}
