#ifndef BST_LIB_H
#define BST_LIB_H

enum typedef tree_err {
    TREE_OK = 0, 
    TREE_EMPTY = 1,
    TREE_MEM = 2,
    TREE_VAL = 3,
}

typedef struct {
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

#endif
