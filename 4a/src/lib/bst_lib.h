#ifndef BST_LIB_H
#define BST_LIB_H

#include <stdio.h>

#include "../client/info_struct.h"

typedef enum {
    TREE_OK = 0, 
    TREE_EMPTY = 1,
    TREE_NULL = 2,
    TREE_MEM = 3,
    TREE_VAL = 4,
    TREE_MAGIC_WORD = 5,
    TREE_SIZE = 6,
    FILE_ERR = 7,
    TREE_EOF = -1,
    TREE_EXIT = -2
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
} BST;

typedef struct {
    TreeNode *node;
    int level;
    int is_last;
} TreePrintFrame;

BST *create_tree();

TreeNode *find_tree(const BST * const, const size_t key);
TreeNode *find_release_tree(TreeNode *, const size_t key, const size_t release);
TreeNode *special_find_tree(const BST * const, const size_t key);

tree_err insert_tree(BST * const, const size_t key, Info * const info);
tree_err delete_tree(BST * const, const size_t key);
tree_err import_tree(BST * const, const char * filename);
tree_err traverse_tree(const BST * const);
tree_err export_tree_svg(const BST * const, const char * const filename);
tree_err export_tree_txt(const BST * const, const char * const filename);
tree_err print_tree(const BST * const);

void free_info(TreeNode * const);
void free_tree(BST * const);
void set_key_and_info(TreeNode * const, const size_t key, Info * const info);

#endif
