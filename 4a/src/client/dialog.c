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

static tree_err dialog_insert(BST * const tree) {
    size_t key, info;
    if (read_positive_number(&key, PROMPT_FOR_KEY) == TREE_EOF) {
        return TREE_EOF;
    }
    if (read_positive_number(&info, PROMPT_FOR_INFO) == TREE_EOF) {
        return TREE_EOF;
    }

    Info *info_ptr = info_create(info);
    tree_err result = TREE_OK;
    if (!info_ptr) {
        result = TREE_MEM;
        goto exit_with_err;
    }

    result = insert_tree(tree, key, info_ptr);
   if (result != TREE_OK) {
        goto exit_with_err;
    }
    
    goto exit_with_err;
    
exit_with_err:
    return result;
}

static tree_err dialog_delete(BST * const tree) {
    if (!tree->root) {
        return TREE_EMPTY;
    }

    size_t key;
    if (read_positive_number(&key, PROMPT_FOR_KEY) == TREE_EOF) {
        return TREE_EOF;
    }

    return delete_tree(tree, key);
}

static TreeNode *search_key_release(TreeNode * const node, size_t * const release) {
    TreeNode *release_node = NULL;
    if (go_to_node(node->left, node->key) != NULL) {
        printf("The element with this key is not the only one. Specify release\n");
        
        if (read_positive_number(release, PROMPT_FOR_RELEASE) == TREE_EOF) {
            return NULL;
        }

        release_node = find_release_tree(node, node->key, *release);
        if (!release_node) {
            return NULL;
        }
    }
    else {
        *release = 1;
    }
    
    return release_node;
}

static tree_err dialog_find(BST * const tree) {
    if (!tree) {
        return TREE_NULL;
    }
    if (!tree->root) {
        return TREE_EMPTY;
    }

    size_t key = 0;
    if (read_positive_number(&key, PROMPT_FOR_KEY) == TREE_EOF) {
        return TREE_EOF;
    }

    TreeNode *node = find_tree(tree, key);
    if (!node || node->key != key) {
        return TREE_VAL;
    }

    size_t release = 1;
    TreeNode *release_node = search_key_release(node, &release);
    if (!release_node) {
        return TREE_VAL;
    }

    printf("\nFound element with key %zu, release %zu:\n", key, release);
    printf("┌──────────────────────┐\n");
    printf("│ Info                 │\n");
    printf("├──────────────────────┤\n");
    printf("│ %-20zu │\n", release_node->info->info);
    printf("└──────────────────────┘\n");

    return TREE_OK;
}

static tree_err dialog_special_find(BST * const tree) {
    if (!tree) {
        return TREE_NULL;
    }
    if (!tree->root) {
        return TREE_EMPTY;
    }

    size_t key = 0;
    if (read_positive_number(&key, PROMPT_FOR_KEY) == TREE_EOF) {
        return TREE_EOF;
    }

    TreeNode *node = special_find_tree(tree, key);
    if (!node) {
        return TREE_VAL;
    }

    printf("\nFound element with key %zu with info %zu\nChecking if he's not the only one.\n", node->key, node->info->info);
    size_t release = 1;
    TreeNode *release_node = search_key_release(node, &release);
    if (!release_node) {
        return TREE_VAL;
    }

    printf("\nFound element with key %zu, release %zu:\n", node->key, release);
    printf("┌──────────────────────┐\n");
    printf("│ Info                 │\n");
    printf("├──────────────────────┤\n");
    printf("│ %-20zu │\n", release_node->info->info);
    printf("└──────────────────────┘\n");

    return TREE_OK;
}

static tree_err dialog_traverse(BST * const tree) {
    return traverse_tree(tree);
}

static tree_err dialog_export_svg(BST * const tree) {
    char *filename = readline(PROMPT_FOR_FILE);
    if (!filename) {
        free(filename);
        return TREE_EOF;
    }

    tree_err err = export_tree_svg(tree, filename);
    free(filename);
    return err;
}

static tree_err dialog_export_txt(BST * const tree) {
    char *filename = readline(PROMPT_FOR_FILE);
    if (!filename) {
        free(filename);
        return TREE_EOF;
    }

    tree_err err = export_tree_txt(tree, filename);
    free(filename);
    return err;
}

static tree_err dialog_exit(__attribute__((__unused__)) BST * const tree) {
    return TREE_EXIT;
}

static tree_err dialog_import(BST * const tree) {
    char *filename = readline(PROMPT_FOR_FILE);
    if (!filename) {
        free(filename);
        return TREE_EOF;
    }
    tree_err err = import_tree(tree, filename);
    free(filename);
    return err;
}

static tree_err dialog_print(BST * const tree) {
    return print_tree(tree);
}

const operation operations[] = {
    {dialog_insert, "Insert element"},
    {dialog_delete, "Delete by key"},
    {dialog_find, "Search by key"},
    {dialog_special_find, "Special search"},
    {dialog_traverse, "Traverse tree"},
    {dialog_export_svg, "Export tree to file .svg"},
    {dialog_export_txt, "Export tree to file .txt"},
    {dialog_import, "Impport tree from file .txt"},
    {dialog_print, "Print tree"},
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
