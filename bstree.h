#ifndef _BSTREE_H
#define _BSTREE_H

#include <stdlib.h>

/**
 * struct bst_node - a node in binary search tree
 *
 * @key:    the key of the node
 * @value:  the value of the node
 * @left:   the pointer to the left child of the node
 * @right:  the pointer to the right child of the node
 */
struct bst_node {
  int              key;
  int              value;
  struct bst_node* left;
  struct bst_node* right;
};

/**
 * bst_get_node - returns a new struct bst_node
 */
struct bst_node *bst_get_node(void) {
  struct bst_node *node = malloc(sizeof(struct bst_node));
  node->left = NULL;
  node->right = NULL;
  return node;
}

/**
 * bst_insert - inserts @key and @value into @tree
 *
 * @tree:   tree to insert @key and @value into
 * @key:    the key to insert into
 * @value:  the value to insert into
 */
void bst_insert(struct bst_node **tree, int key, int value) {
  struct bst_node *walk = *tree;
  struct bst_node *parent = NULL;

  while (walk != NULL){
    if (key == walk->key) return;
    parent = walk;
    walk = key < walk->key ? walk->left : walk->right;
  }

  walk = bst_get_node();
  walk->key = key;
  walk->value = value;

  if(parent == NULL){
    *tree = walk;  
  }
  else if(key < parent->key){
    parent->left = walk;
  }else{
    parent->right = walk;
  }

}

/**
 * bst_erase - erases @key from @tree
 *
 * @tree: tree to erase @key from
 * @key: the key to erase from
 */
void bst_erase(struct bst_node **tree, int key) {
  struct bst_node *walk = *tree;
  struct bst_node *parent = NULL;

  while (walk != NULL && key != walk->key) {
    parent = walk;
    walk = key < walk->key ? walk->left : walk->right;
  }

  if (walk == NULL) return;

  if (walk->left != NULL && walk->right != NULL) {  // case of degree 2
    struct bst_node *tmp = walk;

    for (parent = walk, walk = walk->right; walk->left != NULL; walk = walk->left) parent = walk;

    tmp->key   = walk->key;
    tmp->value = walk->value;
  }

  if (walk->left == NULL && walk->right == NULL) {  // case of degree 0
    if (parent == NULL)             *tree         = NULL;
    else if (parent->left == walk) parent->left   = NULL;
    else                           parent->right  = NULL;
  } else {  // case of degree 1
    if (walk->right == NULL){
      if (parent == NULL)         *tree           = walk->left;
      else if (parent->left == walk) parent->left = walk->left;
      else                        parent->right   = walk->left;
    } else {
      if (parent == NULL)         *tree           = walk->right;
      else if (parent->left == walk) parent->left = walk->right;
      else                        parent->right   = walk->right;
    }
  }

  free(walk);
}

/**
 * bst_inorder - apply @func to each node of @tree
 *
 * @tree: tree to apply @func to each node
 * @func: function to apply to each node of @tree
 */
void bst_inorder(struct bst_node *tree, void (*func)(struct bst_node *)) {
  if (tree != NULL) {
    bst_inorder(tree->left, func);
    func(tree);
    bst_inorder(tree->right, func);
  }
}

#endif /* _BSTREE_H */
