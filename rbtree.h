#ifndef _RBTREE_H
#define _RBTREE_H

#include "include/stack.h"
/*
 Red-Black tree requirements

 1. Every node is either red or black.

 2. The root is black.

 3. All leaves are the same color as the root.

 4. Both children of every red node are black.

 5. Every simple path from a given node to any of its descendent leaves contains the same number of black nodes.

*/
struct rb_node {
  int key;
  int value;
  struct rb_node *left;
  struct rb_node *right;
  enum { RED, BLACK } color;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__)));

static inline struct rb_node *rb_get_node(void) {
  struct rb_node *newNode = malloc(sizeof(struct rb_node));
  newNode->left = NULL;
  newNode->right = NULL;
  newNode->color = RED;
  return newNode;
}

static inline void rb_rotate_left(struct rb_node **restrict root, struct rb_node *restrict node, struct rb_node *restrict parent) {
  struct rb_node *rchild = node->right;
  node->right = rchild->left;
  rchild->left = node;

  if (parent == NULL) *root = rchild;
  else if (parent->left == node) parent->left = rchild;
  else parent->right = rchild;
}

static inline void rb_rotate_right(struct rb_node **restrict root, struct rb_node *restrict node, struct rb_node *restrict parent) {
  struct rb_node *lchild = node->left;
  node->left                 = lchild->right;
  lchild->right           = node;

  if (parent == NULL)         *root         = lchild;
  else if (parent->left == node) parent->left  = lchild;
  else                        parent->right = lchild;
}

extern inline void rb_insert(struct rb_node **restrict tree, int key, int value) {
  register struct rb_node *walk = *tree;
  register struct rb_node *parent;
  register struct rb_node *gparent;
  register struct rb_node *uncle;
  struct stack *path = NULL;

  // Phase 1. shift down and find position
  while (walk != NULL) {
    if (key == walk->key) { 
      destroy(&path); 
      return;
    }
  push(&path, walk);
  walk = key < walk->key ? walk->left : walk->right; 
  }

  walk = rb_get_node();
  walk->key = key;
  walk->value = value;

  if ((parent = top(path)) == NULL) *tree = walk, walk->color = BLACK;
  else if (key < parent->key) parent->left = walk; 
  else parent->right = walk;

  // Phase 2. fixing red & black condition

  while (!empty(path)) {
    if ((parent = pop(&path))->color == BLACK) {
      destroy(&path); 
      return;
    }

    gparent = pop(&path);
    uncle = gparent->right == parent ? gparent->left : gparent->right;

    if (uncle == NULL || uncle->color == BLACK) {         // Rearranging
      if (parent == gparent->left) {
        if (walk == parent->left) {                       // Case of LL
          gparent->color = RED;
          parent->color = BLACK;
          rb_rotate_right(tree, gparent, top(path));
        } else {                                          // Case of LR
          walk->color = BLACK;
          gparent->color = RED;
          rb_rotate_left(tree, parent, gparent);
          rb_rotate_right(tree, gparent, top(path));
        }
      } else {
        if (walk == parent->right) {                      // Case of RR
          gparent->color = RED;
          parent->color = BLACK;
          rb_rotate_left(tree, gparent, top(path));
        } else {                                          // Case of RL
          walk->color = BLACK;
          gparent->color = RED;
          rb_rotate_right(tree, parent, gparent);
          rb_rotate_left(tree, gparent, top(path));
        }
      }

      destroy(&path);
      return;

    } 
    else {                                                // Recoloring
      parent->color = BLACK;
      uncle->color = BLACK;
      gparent->color = empty(path) ? BLACK : RED;
      walk = gparent;
    }
  }
}
extern inline void rb_erase(struct rb_node **restrict tree, int key) {
  register struct rb_node *walk = *tree;
  register struct rb_node *parent;
  register struct rb_node *sibling;
  struct stack *path = NULL;

  while (walk != NULL && key != walk->key) {
    push(&path, walk);
    walk = key < walk->key ? walk->left : walk->right;
  }

  if (walk == NULL) {
    destroy(&path);
    return;
  }

if (walk->left != NULL && walk->right != NULL) {  // case of degree 2
    parent = walk;
    push(&path, walk);
    
    for (walk = walk->left; walk->right != NULL; walk = walk->right) push(&path, walk);

    parent->key = walk->key;
    parent->value = walk->value;
  }

  if (walk->left == NULL && walk->right == NULL) {  // case of degree 0
    if ((parent = top(path)) == NULL)             *tree         = NULL;
    else if (parent->left == walk) parent->left   = NULL;
    else                           parent->right  = NULL;
  } else {  // case of degree 1
    if (walk->right == NULL){
      if ((parent = top(path)) == NULL)         *tree           = walk->left;
      else if (parent->left == walk) parent->left = walk->left;
      else                        parent->right   = walk->left;
    } else {
      if ((parent = top(path)) == NULL)         *tree           = walk->right;
      else if (parent->left == walk) parent->left = walk->right;
      else                        parent->right   = walk->right;
    }
  }

  if (walk->color == RED) {                                                       // Case 1. deleted node is red
    destroy(&path);
    free(walk);
    return;
  }
 
  parent = walk;
  walk = parent->left == NULL ? parent->right : parent->left;
  free(parent);

  if (walk != NULL && walk->color == RED) {                                       // Case 2. successor of the deleted node is red
    walk->color = BLACK;
    destroy(&path);
    return;
  }

  while (!empty(path)){                                                           // double black.
    parent = pop(&path);
    sibling = parent->left == walk ? parent->right : parent->left;

    if (sibling->color == RED) {                                                  // sibling is red
      parent->color = RED;
      sibling->color = BLACK;
      if (parent->left == walk) {
        rb_rotate_left(tree, parent, top(path));
      } else {
        rb_rotate_right(tree, parent, top(path));
      }
      push(&path, sibling);
      sibling = parent->left == walk ? parent->right : parent->left;
    }

    if (sibling->left != NULL && sibling->left->color == RED || sibling->right != NULL && sibling->right->color == RED) {     // sibling is black
      if (parent->left == sibling) {                                              // Case of L
        if (sibling->right != NULL && sibling->right->color == RED) {             // Case of LR
          sibling->right->color = BLACK;
          sibling->color = RED;
          rb_rotate_left(tree, sibling, parent);
          sibling = parent->left;
        }                                                                         // Case of LL
        sibling->color = parent->color;
        sibling->left->color = BLACK;
        parent->color = BLACK;
        rb_rotate_right(tree, parent, top(path));
      } else {                                                                    // Case of R
        if (sibling->left != NULL && sibling->left->color == RED) {               // Case of RL
          sibling->color = RED;
          sibling->left->color = BLACK;
          rb_rotate_right(tree, sibling, parent);
          sibling = parent->right;
        }                                                                         // Case of RR
        sibling->color = parent->color;
        parent->color = BLACK;
        sibling->right->color = BLACK;
        rb_rotate_left(tree, parent, top(path));
      }

      destroy(&path);
      return;
    }

    sibling->color = RED;                                                         // Case of recoloring
    if (parent->color == RED) {
      parent->color = BLACK;
      destroy(&path);
      return;
    } 
    walk = parent;                                                                 // walk pass the black to parent
  }  
}

extern inline void rb_inorder(const struct rb_node *restrict tree, void (*func)(const struct rb_node *restrict)) {
  if (tree != NULL) {
    rb_inorder(tree->left, func);
    func(tree);
    rb_inorder(tree->right, func);
  }
}

#endif
