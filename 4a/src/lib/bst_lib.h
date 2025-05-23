#ifndef BST_LIB_H
#define BST_LIB_H

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
