#ifndef SGT_LIB_H
#define SGT_LIB_H

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
    char *key;
    Info *info;
    struct TreeNode *left;
    struct TreeNode *right;
    struct TreeNode *parent;
    size_t size;
} TreeNode;

typedef struct {
    TreeNode *root;
    double alpha;
    size_t size;
    size_t maxsize; //максильманое число вершин в дереве после последней перебалансировки
} SGT;


SGT* create_tree(const double alpha);

TreeNode *go_to_node(TreeNode * const node, const char * const key);
TreeNode *find_tree(const SGT * const, const char * const key);
TreeNode *find_release_tree(TreeNode *, const char * const, size_t * const current_release, const size_t need_release);
TreeNode *special_find_tree(const SGT * const);

tree_err insert_tree(SGT * const, char * const key, Info * const info);
tree_err delete_tree(SGT * const, const char * const key);
tree_err import_tree(SGT * const, const char * const filename);
tree_err traverse_tree(const SGT * const, const char * const min_key);
tree_err export_tree_svg(const SGT * const, const char * const filename);
tree_err export_tree_txt(const SGT * const, const char * const filename);
tree_err print_tree(const SGT * const);

int cmp(const char * const, const char * const);

char *read_row_from_file(FILE * const);

void free_info(TreeNode * const);
void free_tree(SGT * const);

#endif
