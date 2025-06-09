#ifndef BST_LIB_H
#define BST_LIB_H

#include <stdio.h>

#include "../client/info_struct.h"

typedef enum {
    TREE_OK = 0, 
    TREE_EMPTY = 1,
    TREE_NULL = 2,
    TREE_MEM = 3,
    TREE_VAL = 4
} tree_err;

typedef struct TreeNode {
    size_t key;
    Info *info;
    struct TreeNode *left;
    struct TreeNode *right;
    struct TreeNode *parent;
} TreeNode;

typedef struct {
    TreeNode *root;
    size_t size;
} BST;

BST *create_tree();

TreeNode *create_tree_node(const size_t key, Info * const info);
TreeNode *find_tree(const BST * const, const size_t key);
TreeNode *find_release_tree(TreeNode *, const size_t key, const size_t release);
TreeNode *special_find_tree(const BST * const, const size_t key);

tree_err insert_tree(BST * const, const size_t key, Info * const info);
tree_err delete_tree(BST * const, const size_t key);
tree_err walk_tree(const BST * const);

void free_key_and_info(TreeNode * const);
void free_tree(BST * const);
void set_key_and_info(TreeNode * const, const size_t key, Info * const info);

#endif
