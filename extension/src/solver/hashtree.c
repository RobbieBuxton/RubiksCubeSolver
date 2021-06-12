#include "hashtree.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

HashTree *new_hash_tree(void) {
    // Attempt to create a tree.
    HashTree *tree = (HashTree *) malloc(sizeof(HashTree));
    if (!tree) {
        return NULL;
    }

    // Populate the tree's fields.
    tree->count = 0;
    tree->root  = NULL;

    return tree;
}

// Empty out a node.
static void clear_node(TreeNode *node) {
    node->colour      = RED_NODE;
    node->hash        = 0u;
    node->any_ptr     = NULL;
    node->parent      = NULL;
    node->left_child  = NULL;
    node->right_child = NULL;
}

static TreeNode *new_node(const uint64_t hash) {
    TreeNode *node = (TreeNode *) malloc(sizeof(TreeNode));
    if (!node) {
        // Calloc failed.
        return NULL;
    }

    // Initialise fields.
    clear_node(node);

    node->hash = hash;

    return node;
}

static bool free_node(TreeNode *node) {
    if (!node) {
        return false;
    }

    // First, free the children:
    if (node->left_child) {
        assert(free_node(node->left_child));
    }

    if (node->right_child) {
        assert(free_node(node->right_child));
    }

    // Then, free the given node.
    free(node);

    return true;
}

bool free_hash_tree(HashTree *tree) {
    if (!tree) {
        return false;
    }

    // First, free the nodes of the tree
    if (tree->root && !free_node(tree->root)) {
        fprintf(stderr, "Warning: nodes from tree at addr %zu failed to be freed!\n", (size_t) tree);
    }

    // Then free the tree itself.
    free(tree);

    return true;
}

// Is this node a left child?
static bool is_left_child(TreeNode *node) {
    return node->parent && ((node->parent)->left_child == node);
}

// Is this node a right child?
static bool is_right_child(TreeNode *node) {
    return node->parent && ((node->parent)->right_child == node);
}

// Reparent a node, and return the ejected node.
// If the reparenting fails for any reason, return NULL.
static TreeNode *reparent(HashTree *tree, TreeNode *current_node, TreeNode *new_node) {
    // Ejected node.
    TreeNode *cur = current_node;
    TreeNode *par = cur->parent;

    if (!par) {
        // At root.
        tree->root = new_node;
        new_node->parent = NULL;
    } else if (is_left_child(current_node)) {
        // Current node is left child of parent.
        par->left_child = new_node;
        new_node->parent = par;
        cur->parent = NULL;
    } else if (is_right_child(current_node)) {
        // Current node is right child of parent.
        par->right_child = new_node;
        new_node->parent = par;
        cur->parent = NULL;
    } else {
        fprintf(stderr, "Reparenting failed! Node at %zu does not have node at %zu as parent.\n", (size_t) cur, (size_t) par);
        return NULL;
    }

    return cur;
}

// Pre: current has a right child.
static void rotate_left(HashTree *tree, TreeNode *current) {
    assert(current->right_child);

    TreeNode *right = current->right_child;
    TreeNode *rights_left_child = right->left_child;

    // Set current's right to right's leftt child:
    current->right_child = rights_left_child;

    // Reparent right.
    current = reparent(tree, current, right);
    assert(current);

    // Set right's left child to current.
    right->left_child = current;
}

// Pre: current has a left child.
static void rotate_right(HashTree *tree, TreeNode *current) {
    assert(current->left_child);

    TreeNode *left = current->left_child;
    TreeNode *lefts_right_child = left->right_child;

    // Set current's left to left's right child:
    current->left_child = lefts_right_child;

    // Reparent left.
    current = reparent(tree, current, left);
    assert(current);

    // Set left's right child to current.
    left->right_child = current;
}

// Cases for post-insertion rebalancing and fixing of the tree.
static void case_1(HashTree *tree, TreeNode *current);
static void case_2(HashTree *tree, TreeNode *current);
static void case_3(HashTree *tree, TreeNode *current);
static void case_4(HashTree *tree, TreeNode *current);
static void case_5(HashTree *tree, TreeNode *current);

static void case_1(HashTree *tree, TreeNode *current) {
    if (!current->parent) {
        current->colour = BLACK_NODE;
    } else {
        case_2(tree, current);
    }
}

static void case_2(HashTree *tree, TreeNode *current) {
    assert(current->parent);

    if (current->parent->colour != BLACK_NODE) {
        case_3(tree, current);
    }
}

static void case_3(HashTree *tree, TreeNode *current) {
    TreeNode *par, *gpar, *unc;

    par = current->parent;
    assert(par);

    gpar = par->parent;
    assert(gpar);

    if (is_left_child(par)) {
        unc = gpar->right_child;
    } else if (is_right_child(par)) {
        unc = gpar->left_child;
    } else {
        fprintf(stderr, "[Case 3]: Error, parent is in an invalid state!\n");
        assert(false);
    }

    if (unc->colour == RED_NODE) {
        par->colour = BLACK_NODE;
        unc->colour = BLACK_NODE;
        gpar->colour = RED_NODE;

        case_1(tree, gpar);
    } else {
        case_4(tree, current);
    }
}

static void case_4(HashTree *tree, TreeNode *current) {
    TreeNode *par = current->parent;
    assert(par);

    if (is_left_child(par) && is_right_child(current)) {
        // 4a
        rotate_left(tree, par);
        case_5(tree, par);
    } else if (is_right_child(par) && is_left_child(current)) {
        // 4b
        rotate_right(tree, par);
        case_5(tree, par);
    } else {
        case_5(tree, current);
    }
}

static void case_5(HashTree *tree, TreeNode *current) {
    TreeNode *par, *gpar;

    par = current->parent;
    assert(par);

    gpar = par->parent;
    assert(gpar);

    par->colour = BLACK_NODE;
    gpar->colour = RED_NODE;

    if (is_left_child(current)) {
        // 5a
        rotate_right(tree, gpar);
    } else if (is_right_child(current)) {
        // 5b
        rotate_left(tree, gpar);
    } else {
        fprintf(stderr, "[Case 5] Error, the tree is in an invalid state!\n");
        assert(false);
    }
}

bool add_to_hash_tree(HashTree *tree, uint64_t hash) {
    if (hash == 0ul) {
        fprintf(stderr, "Warning, zero hash!\n");
        return false;
    }

    if (tree->root) {
        TreeNode *curr_ptr = tree->root;
        TreeNode *next = NULL;
        TreeNode **where = NULL;

        while (curr_ptr) {
            // Traverse the tree until we find a place to add the node.
            if (hash > curr_ptr->hash) {
                next = curr_ptr->right_child;

                if (next) {
                    curr_ptr = next;
                } else {
                    where = &(curr_ptr->right_child);
                    curr_ptr = next;
                }
            } else if (hash < curr_ptr->hash) {
                next = curr_ptr->left_child;

                if (next) {
                    curr_ptr = next;
                } else {
                    where = &(curr_ptr->left_child);
                    curr_ptr = next;
                }
            } else {
                // Hash is already in tree.
                return false;
            }
        }

        // Insert and fix.
        *where = new_node(hash);
        if (!where) {
            return false;
        }

        case_1(tree, *where);
    } else {
        // Insert and fix.
        tree->root = new_node(hash);
        if (!tree->root) {
            return false;
        }

        tree->root->hash = hash;

        case_1(tree, tree->root);
    }

    // Increment the count.
    ++(tree->count);

    return true;
}

static TreeNode *navigate_tree(const HashTree *tree, uint64_t hash) {
    // Search for a hash.
    TreeNode *curr_ptr = tree->root;

    while (curr_ptr) {
        if (hash > curr_ptr->hash) {
            curr_ptr = curr_ptr->right_child;
        } else if (hash < curr_ptr->hash) {
            curr_ptr = curr_ptr->left_child;
        } else {
            return curr_ptr;
        }
    }

    // Not found.
    return NULL;
}

bool modify_pointer_in_hash_tree(HashTree *tree, uint64_t hash, void *ptr) {
    TreeNode *curr_ptr = navigate_tree(tree, hash);

    if (curr_ptr) {
        curr_ptr->any_ptr = ptr;
        return true;
    } else {
        return false;
    }
}

void *get_pointer_from_hash_tree(const HashTree *tree, const uint64_t hash) {
    TreeNode *curr_ptr = navigate_tree(tree, hash);

    if (curr_ptr) {
        return curr_ptr->any_ptr;
    } else {
        return NULL;
    }
}

bool query_hash_tree(const HashTree *tree, const uint64_t hash) {
    TreeNode *curr_ptr = navigate_tree(tree, hash);
    return !!curr_ptr;
}

