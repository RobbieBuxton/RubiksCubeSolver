#include "cubestate.h"
#include "movequeue.h"
#include "solver.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool solve(CubeState *start, int *move_count, Movement *solution) {
    MovePriorityQueue *queue = new_move_priority_queue(100);
    MoveQueueNode query_result;
    add_to_move_priority_queue(queue, start, estimate_cost(start));
    HashTree* visitedHashes = new_hash_tree();
    int count = 0;
    int count2 = 0;

    while(queue->count > 0) {

        // Get next state from the queue
        if (!poll_move_priority_queue(queue, &query_result)) {
            free_hash_tree(visitedHashes);
            free_move_priority_queue(queue);

            return false;
        }
        count2++;

        if (query_result.state.history_count > count) {
            // printf("%ld \n", query_result.state.history_count);
            printf("%d count\t", ++count);
            printf("%d count\n", count2);
            // printCubeState(&(query_result.state));
        }

        if (!visit(&(query_result.state), visitedHashes)) {
            // printf("visited before\n");
            continue;
        }

        if (solved(&(query_result.state))) {
            *move_count = query_result.state.history_count;
            memcpy(solution, query_result.state.history, sizeof(query_result.state.history));

            free_hash_tree(visitedHashes);
            free_move_priority_queue(queue);

            return true;
        }

        expand_all_moves(&(query_result.state), queue, visitedHashes);
    }

    free_hash_tree(visitedHashes);
    free_move_priority_queue(queue);

    return false;
}

// static double spot_colour_heuristic(CubeState *state) {
//     double count = 36;
//     for (int face = TOP; face < FACES; face++) {
//         for (int row = 0; row < SIDE_LENGTH; row++) {
//             for (int col = 0; col < SIDE_LENGTH; col++) {
//                 if (MATCHES_CENTRE(face, row, col, state)) {
//                     count--;
//                 }
//             }
//         }
//     }
//     return count / 12;
// }

// static double edge_piece_heuristic(CubeState *state) {
//     double count = 12;
//     if (MATCHES_CENTRE(TOP, 0, 1, state) && MATCHES_CENTRE(BACK, 0, 1, state)) count--;
//     if (MATCHES_CENTRE(TOP, 1, 0, state) && MATCHES_CENTRE(LEFT, 0, 1, state)) count--;
//     if (MATCHES_CENTRE(TOP, 1, 2, state) && MATCHES_CENTRE(RIGHT, 0, 1, state)) count--;
//     if (MATCHES_CENTRE(TOP, 2, 1, state) && MATCHES_CENTRE(FRONT, 0, 1, state)) count--;

//     if (MATCHES_CENTRE(LEFT, 1, 0, state) && MATCHES_CENTRE(BACK, 1, 2, state)) count--;
//     if (MATCHES_CENTRE(BACK, 1, 0, state) && MATCHES_CENTRE(RIGHT, 1, 2, state)) count--;
//     if (MATCHES_CENTRE(RIGHT, 1, 0, state) && MATCHES_CENTRE(FRONT, 1, 2, state)) count--;
//     if (MATCHES_CENTRE(FRONT, 1, 0, state) && MATCHES_CENTRE(LEFT, 1, 2, state)) count--;

//     if (MATCHES_CENTRE(BOTTOM, 0, 1, state) && MATCHES_CENTRE(FRONT, 2, 1, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 1, 0, state) && MATCHES_CENTRE(LEFT, 2, 1, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 1, 2, state) && MATCHES_CENTRE(RIGHT, 2, 1, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 2, 1, state) && MATCHES_CENTRE(BACK, 2, 1, state)) count--;

//     return count / 4;
// }

// static double corner_piece_heuristic(CubeState *state) {
//     double count = 4;
//     if (MATCHES_CENTRE(TOP, 0, 0, state) && MATCHES_CENTRE(BACK, 0, 2, state) && MATCHES_CENTRE(LEFT, 0, 0, state)) count--;
//     if (MATCHES_CENTRE(TOP, 0, 2, state) && MATCHES_CENTRE(RIGHT, 0, 2, state) && MATCHES_CENTRE(BACK, 0, 0, state)) count--;
//     if (MATCHES_CENTRE(TOP, 2, 2, state) && MATCHES_CENTRE(FRONT, 0, 2, state) && MATCHES_CENTRE(RIGHT, 0, 0, state)) count--;
//     if (MATCHES_CENTRE(TOP, 2, 0, state) && MATCHES_CENTRE(LEFT, 0, 2, state) && MATCHES_CENTRE(FRONT, 0, 0, state)) count--;

//     if (MATCHES_CENTRE(BOTTOM, 0, 0, state) && MATCHES_CENTRE(LEFT, 2, 2, state) && MATCHES_CENTRE(FRONT, 2, 0, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 0, 2, state) && MATCHES_CENTRE(FRONT, 2, 2, state) && MATCHES_CENTRE(RIGHT, 2, 0, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 2, 2, state) && MATCHES_CENTRE(RIGHT, 2, 2, state) && MATCHES_CENTRE(BACK, 2, 0, state)) count--;
//     if (MATCHES_CENTRE(BOTTOM, 2, 0, state) && MATCHES_CENTRE(BACK, 2, 2, state) && MATCHES_CENTRE(LEFT, 2, 0, state)) count--;

//     return count / 4;
// }

static double misplaced_pieces_heuristic(CubeState *state) {
    int max = 0;
    int min = 8;
    for (Face face = 0; face < FACES; face++) {
        int count = 0;
        if (!MATCHES_CENTRE(face, 0, 1, state)) count++;
        if (!MATCHES_CENTRE(face, 1, 0, state)) count++;
        if (!MATCHES_CENTRE(face, 1, 2, state)) count++;
        if (!MATCHES_CENTRE(face, 2, 1, state)) count++;
        if (MISPLACED_CORNER(face, 0, 0, state)) count++;
        if (MISPLACED_CORNER(face, 0, 2, state)) count++;
        if (MISPLACED_CORNER(face, 2, 0, state)) count++;
        if (MISPLACED_CORNER(face, 2, 2, state)) count++;
        if (max < count) max = count;
        if (min > count) min = count;
    }
    return max + min;
}

double heuristic(CubeState *state) {
    double h = 0;
    if (solved(state)) {
        return 0;
    }
    // add in more future heuristics
    // h += spot_colour_heuristic(state) * 0;
    // h += edge_piece_heuristic(state) * 0;
    // h += corner_piece_heuristic(state) * 0;
    h += misplaced_pieces_heuristic(state);
    return h;
}

double estimate_cost(CubeState *state) {
    return heuristic(state) + state->history_count;
}

bool expand_all_moves(CubeState *current, MovePriorityQueue *queue, HashTree *visitedHashes) {
    if (current->history_count == MAXIMUM_MOVEMENTS) {
        return false;
    }
    CubeState next;
    Movement movement;
    for (int direction = 0; direction < 3; direction++) {
        movement.direction = direction;
        for (int face = 0; face < 6; face++) {
            movement.face = face;
            next = apply_movement(current, movement);
            if (!query_hash_tree(visitedHashes, hash_cubestate(&next))) {
                add_to_move_priority_queue(queue, &next, estimate_cost(&next));
            }
        }
    }
    return true;
}

bool visit(CubeState *current, HashTree *visitedHashes) {
    return add_to_hash_tree(visitedHashes, hash_cubestate(current));
}


// ----------------------------------- tree implementation -------------------------------------

HashTree *new_hash_tree(void) {
    // Attempt to create a map.
    HashTree *map = (HashTree *) malloc(sizeof(HashTree));
    if (!map) {
        return NULL;
    }

    // Populate the map's fields.
    map->count = 0;
    map->root  = NULL;

    return map;
}

// Empty out a node.
static void clear_node(MapNode *symbol) {
    symbol->colour      = RED_NODE;
    symbol->hash        = 0u;
    symbol->parent      = NULL;
    symbol->left_child  = NULL;
    symbol->right_child = NULL;
}

static MapNode *new_node(const uint64_t hash) {
    MapNode *symbol = (MapNode *) malloc(sizeof(MapNode));
    if (!symbol) {
        // Calloc failed.
        return NULL;
    }

    // Initialise fields.
    clear_node(symbol);

    symbol->hash = hash;

    return symbol;
}

static bool free_node(MapNode *symbol) {
    if (!symbol) {
        return false;
    }

    // First, free the children:
    if (symbol->left_child) {
        assert(free_node(symbol->left_child));
    }

    if (symbol->right_child) {
        assert(free_node(symbol->right_child));
    }

    // Then, free the given node.
    free(symbol);

    return true;
}

bool free_hash_tree(HashTree *map) {
    if (!map) {
        return false;
    }

    // First, free the nodes of the tree
    if (map->root && !free_node(map->root)) {
        fprintf(stderr, "Warning: symbols from map at addr %zu failed to be freed!\n", (size_t) map);
    }

    // Then free the map itself.
    free(map);

    return true;
}

// Is this node a left child?
static bool is_left_child(MapNode *node) {
    return node->parent && ((node->parent)->left_child == node);
}

// Is this node a right child?
static bool is_right_child(MapNode *node) {
    return node->parent && ((node->parent)->right_child == node);
}

// Reparent a node, and return the ejected node.
// If the reparenting fails for any reason, return NULL.
static MapNode *reparent(HashTree *map, MapNode *current_node, MapNode *new_node) {
    // Ejected node.
    MapNode *cur = current_node;
    MapNode *par = cur->parent;

    if (!par) {
        // At root.
        map->root = new_node;
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
static void rotate_left(HashTree *map, MapNode *current) {
    assert(current->right_child);

    MapNode *right = current->right_child;
    MapNode *rights_left_child = right->left_child;

    // Set current's right to right's leftt child:
    current->right_child = rights_left_child;

    // Reparent right.
    current = reparent(map, current, right);
    assert(current);

    // Set right's left child to current.
    right->left_child = current;
}

// Pre: current has a left child.
static void rotate_right(HashTree *map, MapNode *current) {
    assert(current->left_child);

    MapNode *left = current->left_child;
    MapNode *lefts_right_child = left->right_child;

    // Set current's left to left's right child:
    current->left_child = lefts_right_child;

    // Reparent left.
    current = reparent(map, current, left);
    assert(current);

    // Set left's right child to current.
    left->right_child = current;
}

// Cases for post-insertion rebalancing and fixing of the tree.
static void case_1(HashTree *map, MapNode *current);
static void case_2(HashTree *map, MapNode *current);
static void case_3(HashTree *map, MapNode *current);
static void case_4(HashTree *map, MapNode *current);
static void case_5(HashTree *map, MapNode *current);

static void case_1(HashTree *map, MapNode *current) {
    if (!current->parent) {
        current->colour = BLACK_NODE;
    } else {
        case_2(map, current);
    }
}

static void case_2(HashTree *map, MapNode *current) {
    assert(current->parent);

    if (current->parent->colour != BLACK_NODE) {
        case_3(map, current);
    }
}

static void case_3(HashTree *map, MapNode *current) {
    MapNode *par, *gpar, *unc;

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

        case_1(map, gpar);
    } else {
        case_4(map, current);
    }
}

static void case_4(HashTree *map, MapNode *current) {
    MapNode *par = current->parent;
    assert(par);

    if (is_left_child(par) && is_right_child(current)) {
        // 4a
        rotate_left(map, par);
        case_5(map, par);
    } else if (is_right_child(par) && is_left_child(current)) {
        // 4b
        rotate_right(map, par);
        case_5(map, par);
    } else {
        case_5(map, current);
    }
}

static void case_5(HashTree *map, MapNode *current) {
    MapNode *par, *gpar;

    par = current->parent;
    assert(par);

    gpar = par->parent;
    assert(gpar);

    par->colour = BLACK_NODE;
    gpar->colour = RED_NODE;

    if (is_left_child(current)) {
        // 5a
        rotate_right(map, gpar);
    } else if (is_right_child(current)) {
        // 5b
        rotate_left(map, gpar);
    } else {
        fprintf(stderr, "[Case 5] Error, the tree is in an invalid state!\n");
        assert(false);
    }
}

bool add_to_hash_tree(HashTree *map, uint64_t hash) {
    if (hash == 0ul) {
        fprintf(stderr, "Warning, zero hash!\n");
        return false;
    }

    if (map->root) {
        MapNode *curr_ptr = map->root;
        MapNode *next = NULL;
        MapNode **where = NULL;

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

        case_1(map, *where);
    } else {
        // Insert and fix.
        map->root = new_node(hash);
        if (!map->root) {
            return false;
        }

        map->root->hash = hash;

        case_1(map, map->root);
    }

    // Increment the count.
    ++(map->count);

    return true;
}

bool query_hash_tree(const HashTree *map, const uint64_t hash) {

    // Search for a hash.
    MapNode *curr_ptr = map->root;

    while (curr_ptr) {
        if (hash > curr_ptr->hash) {
            curr_ptr = curr_ptr->right_child;
        } else if (hash < curr_ptr->hash) {
            curr_ptr = curr_ptr->left_child;
        } else {
            return true;
        }
    }

    // Not found.
    return false;
}
