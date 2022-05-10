#ifndef _AVLTREE_H
#define _AVLTREE_H

#include <stdint.h>

#include "include/stack.h"

struct avl_node {
  int key;
  int value;
  struct avl_node *left;
  struct avl_node *right;
  uint32_t height;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__)));

static inline struct avl_node *avl_get_node(void){
  struct avl_node *newNode = malloc(sizeof(struct avl_node));
  newNode->left = NULL;
  newNode->right = NULL;
  newNode->height = 1;
  return newNode;
}

static inline uint32_t max(uint32_t a, uint32_t b) { return a < b ? b : a; }

static inline uint32_t height(struct avl_node *tree) { return tree == NULL ? 0 : tree->height; }

static inline void avl_rotate_left(struct avl_node **restrict root, struct avl_node *restrict x, struct avl_node *restrict parent) {
  struct avl_node *rchild = x->right;
  x->right = rchild->left;
  rchild->left = x;

  if (parent == NULL) *root = rchild;
  else if (parent->left == x) parent->left = rchild;
  else parent->right = rchild;
}

static inline void avl_rotate_right(struct avl_node **restrict root, struct avl_node *restrict x, struct avl_node *restrict parent) {
  struct avl_node *lchild = x->left;
  x->left                 = lchild->right;
  lchild->right           = x;

  if (parent == NULL)         *root         = lchild;
  else if (parent->left == x) parent->left  = lchild;
  else                        parent->right = lchild;
}

extern inline void avl_insert(struct avl_node **restrict tree, int key, int value) {
  struct stack *path = NULL;
  register struct avl_node *walk = *tree;
  struct avl_node *parent = NULL;
  struct avl_node *x = NULL;

  while (walk != NULL) {
    if (key == walk->key) { destroy(&path); return; }
    push(&path, walk);
    walk = key < walk->key ? walk->left : walk->right;
  }

  walk = avl_get_node();
  walk->key = key;
  walk->value = value;

  if ((parent = top(path)) == NULL) *tree = walk;
  else if (key < parent->key) parent->left = walk;
  else parent->right = walk;

  while (!empty(path)) {
    walk = pop(&path);
    walk->height = 1 + max(height(walk->left), height(walk->right));
    int bf = height(walk->left) - height(walk->right);
    if (1 < bf || bf < -1) {
      x = walk;
      parent = top(path);
      break;
    }
  }

  if (x == NULL) return;

  // Balenced factor > 1 ( L child case)
  if (height(x->left) > 1 + height(x->right)) {
    if (height(x->left->left) >= height(x->left->right)) { // Case of LL
      push(&path, x->left);
      push(&path, x);
      avl_rotate_right(tree, x, parent);
    } else { // Case of LR
      push(&path, x->left->right);
      push(&path, x->left);
      push(&path, x);
      avl_rotate_left(tree, x->left, x);
      avl_rotate_right(tree, x, parent);
    }
  } else {
    if (height(x->right->right) >= height(x->right->left)) { // Case of RR
      push(&path, x->right);
      push(&path, x);
      avl_rotate_left(tree, x, parent);
    }else { // Case of RL
      push(&path, x->right->left);
      // push(&path, x);
      push(&path, x->right);
      push(&path, x);
      avl_rotate_right(tree, x->right, x);
      avl_rotate_left(tree, x, parent);
    }
  }

  while (!empty(path)) {
    walk = pop(&path);
    walk->height = 1 + max(height(walk->left), height(walk->right));
  }
}

extern inline void avl_erase(struct avl_node **restrict tree, int key) {
  // Phase 1. Perform Standard BST delete
  struct stack *path = NULL;
  register struct avl_node *walk = *tree;
  struct avl_node *parent = NULL;
  struct avl_node *x = NULL;

  while (walk != NULL && key != walk->key) {
    push(&path, walk);
    parent = walk;
    walk = key < walk->key ? walk->left : walk->right;
  }

  if (walk == NULL) {
    destroy(&path);
    return;
  }
  if (walk->left != NULL && walk->right != NULL) {  // case of degree 2
    parent = walk;
    push(&path, walk);
    
    if (height(walk->left) <= height(walk->right)) {
      for (walk = walk->right; walk->left != NULL; walk = walk->left) push(&path, walk);
    } else {
      for (walk = walk->left; walk->right != NULL; walk = walk->right) push(&path, walk);
    }

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
  
  free(walk);

  //Phase 2. 
  
  while (!empty(path)) {
    walk = pop(&path);
    walk->height = 1 + max(height(walk->left), height(walk->right));
    int bf = height(walk->left) - height(walk->right);
    if (1 < bf || bf < -1) {
      x = walk;
      parent = top(path);
      break;
    }
  }

  if (x == NULL) return;

  // Balenced factor > 1 ( L child case)
  if (height(x->left) > 1 + height(x->right)) {
    if (height(x->left->left) >= height(x->left->right)) { // Case of LL
      push(&path, x->left);
      push(&path, x);
      avl_rotate_right(tree, x, parent);
    } else { // Case of LR
      push(&path, x->left->right);
      push(&path, x->left);
      push(&path, x);
      avl_rotate_left(tree, x->left, x);
      avl_rotate_right(tree, x, parent);
    }
  } else {
    if (height(x->right->right) >= height(x->right->left)) { // Case of RR
      push(&path, x->right);
      push(&path, x);
      avl_rotate_left(tree, x, parent);
    }else { // Case of RL
      push(&path, x->right->left);
      push(&path, x->right);
      push(&path, x);
      avl_rotate_right(tree, x->right, x);
      avl_rotate_left(tree, x, parent);
    }
  }

  while (!empty(path)) {
    walk = pop(&path);
    walk->height = 1 + max(height(walk->left), height(walk->right));
  }
}

void avl_inorder(struct avl_node *restrict tree, void (*func)(struct avl_node *restrict)) {
  if (tree != NULL) {
    avl_inorder(tree->left, func);
    func(tree);
    avl_inorder(tree->right, func);
  }
}

#endif /* _AVLTREE_H */
