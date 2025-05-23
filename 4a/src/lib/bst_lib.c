#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst_lib.h"
#include "../client/info_struct.h"

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
