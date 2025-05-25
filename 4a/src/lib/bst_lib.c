#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst_lib.h"
#include "../client/info_struct.h"
#include "../client/queue_lib.h"
#include "../client/stack_lib.h"

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

void free_tree(BST * const tree) {
    if (!tree->root) {
        return;
    }

    Queue *queue = queue_create(tree->size);

    if (queue_push(queue, tree->root) != QUEUE_OK) {
        return;
    }

    while (queue_not_empty(queue) != 0) {
        TreeNode *node = queue_pop(queue);
        if (!node->left) {
            queue_push(queue, node->left);
        }
        if (!node->right) {
            queue_push(queue, node->right);
        }
        
        free(node);
    }

    queue_free(queue);
    tree->root = NULL;
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

TreeNode *find_tree(const BST * const tree, const size_t key) {
    if (!tree) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }

    TreeNode *current = tree->root;
    while (current) {
        if (key > current->key) {
            current = current->right;
        }
        else if (key < current->key) {
            current = current->left;
        } else {
            break;
        }
    }

    return current;
}

TreeNode *find_release_tree(const TreeNode * const current, const size_t key, const size_t release) {
    if (!tree) {
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
        else if (current->left->key == key) {
            count++;
            current = current->left;
        } else {
            break;
        }
    }

    return NULL;
}

TreeNode *special_find_tree(const BST * const tree, const size_t key) {
    if (!tree) {
        return NULL;
    }
    if (!key) {
        return NULL;
    }

    TreeNode *current = tree->root;
    while (current) {
        if (key > current->key) {
            current = current->right;
        }
        else if (key < current->key) {
            current = current->left;
        } else {
            break;
        }
    }

    return current;
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
    tree->size++;
    
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
    
    tree->size--;
    return TREE_OK;
}

tree_err walk_tree(const BST * const tree) {
    if (!tree) {
        return TREE_NULL;
    }
    
    Stack *stack = stack_create(tree->size);
    if (!stack) {
        return TREE_VAL;
    }

    TreeNode *current = tree->root;
    while (current) {
        while (current->left) {
            stack_push(stack, current);
            current = current->left;
        }

        while (stack_not_empty(stack) != 0) {
            current = stack_pop(stack);
            //printf(current)
            if (current->right) {
                break;
            }
        }
    }

    return TREE_OK;
}
