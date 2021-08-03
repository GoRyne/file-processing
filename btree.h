#ifndef _BTREE_H
#define _BTREE_H

#include <string.h>

#include "include/stack.h"

struct btree_node {
  const void              **keys;
        void              **values;
        struct btree_node **children;
        size_t            nmemb;
};

static inline struct btree_node *btree_get_node(const size_t order) {
  struct btree_node *node = malloc(sizeof(struct btree_node));
  node->keys              = malloc(sizeof(void *)*(order-1));
  node->values            = malloc(sizeof(void *)*(order-1));
  node->children          = malloc(sizeof(struct btree_node *)*order);
  node->nmemb             = 0;
  return node;
}

static inline size_t binsearch(const void *restrict key, const void **restrict base, const size_t nmemb, bool (*less)(const void *, const void *)) {
  // [2, 3, 5, 7, 9], 3 => 1
  // [2, 3, 5, 7, 9], 0 => 0
  // [2, 3, 5, 7, 9], 4 => 2
  // [2, 3, 5, 7, 9], 11 => 5
  size_t init, curr, last;
  init = 0;
  last = nmemb;

  while (init < last) {
    curr = (init + last) / 2;
    if (less(key, base[curr])) {
      last = curr;
    } else if (less(base[curr], key)) {
      init = curr + 1;
    } else {
      return curr;
    }
  }
  return init;
}

extern inline void btree_insert(struct btree_node **restrict tree, const size_t order, const void *key, void *value, bool (*less)(const void *, const void *)) {
  // memcpy(&node->keys[7], &node-keys[8], sizeof(void *)*3);
}

extern inline void btree_erase(struct btree_node **restrict tree, const size_t order, const void *key, bool (*less)(const void *, const void *)) {
  // TODO
}

extern inline void btree_inorder(struct btree_node *restrict tree, void (*func)(const struct btree_node *restrict)) {
  // TODO
}

#endif
