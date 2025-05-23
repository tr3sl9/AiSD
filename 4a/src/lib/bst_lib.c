#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst_lib.h"
#include "../client/info_struct.h"

BST *create_tree() {
    BST *tree = (BST*)calloc(1, sizeof(BST));
    if (!tree) {
        return NULL;
    }

    return tree;
}

TreeNode *create_tree_node(const size_t key, const Info * const info) {
    TreeNode *node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (!node) {
        return NULL;
    }

    node->key = key;
    if (!node->key) {
        free_node(node);
        return NULL;
    }
    
    node->info = info;
    if(!node->info) {
        free_node(node);
        return NULL;
    }

    return node;
}

void free_key_and_info(TreeNode * const node) {
    free(node->key);
    info_free(node->info);
    return;
}

void set_key_and_info(TreeNode * const node, const size_t key, const Info * const info) {
    if (!node || !key || !info) {
        return TREE_VAL;
    }

    node->key = key;
    node->info = info;
    return;
}

tree_err insert_tree(BST * const tree, const size_t key, const Info * const info) {
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

    TreeNode *current = tree->root;
    if (!current) {
        tree->root = new_node;
        return TREE_OK;
    }
    TreeNode *current_parent = NULL;

    while (!cur) {
        current_parent = current;
        if (key > current->key) {
            current = current->right;
        } else {
            current = current->left;
        }
    }
    
    if (key > current_parent->key) {
        current_parent->right = new_node;
    } else {
        current_parent->left = new_node;
    }
    new_node->parent = current_parent;
    
    return TREE_OK;
}

tree_err delete_tree(BST * const tree, const size_t key) {
    if (!tree) {
        return TREE_NULL;
    }
    if (!key) {
        return TREE_VAL;
    }

    TreeNode *current = tree->root;
    if (!current) {
        return TREE_EMPTY;
    }
    TreeNode *current_parent = NULL;

    while (!current) {
        if (key > current->key) {
            current_parent = current;
            current = current->right;
        } else if (key < current->key) {
            current_parent = current;
            current = current->left;
        } else {
            break;
        }
    }

    if (!current) {
        return TREE_VAL;
    }

    if (current->left && current->right) {
        TreeNode *child = current->right;
        TreeNode *child_parrent = current;
        while (child->left) {
            child_parent = child;
            child = child->left;
        }
        free_key_and_info(current);
        set_key_and_info(current, child->key, child->info);
        child_parrent->left = child->right;
        free(child);
    } else {
        free_key_and_info(current);
        TreeNode *child = (current->left) ? current->left : current->right;
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
    }

    return TREE_OK;
}
