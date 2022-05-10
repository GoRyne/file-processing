#ifndef _BTREE_H
#define _BTREE_H

#include <string.h>
#include "include/stack.h"


struct btree_node {
  const void              **keys;
        void              **values;
        struct btree_node **children;
        size_t            nmemb;
} __attribute__((aligned(__SIZEOF_POINTER__)));

static inline struct btree_node *btree_alloc(const size_t order) {
  struct btree_node *node = malloc(sizeof(struct btree_node));
  node->keys              = malloc(__SIZEOF_POINTER__*(order-1));
  node->values            = malloc(__SIZEOF_POINTER__*(order-1));
  node->children          = malloc(__SIZEOF_POINTER__*order);
  node->nmemb             = 0;
  return node;
}

static inline void btree_free(struct btree_node *node) {
  free(node->keys);
  free(node->values);
  free(node->children);
  free(node);
}

static inline size_t binsearch(const void *restrict key, const void **restrict base, const size_t nmemb, bool (*less)(const void *, const void *)) {
  register size_t idx;
  register size_t lo = 0;
  register size_t hi = nmemb;

  while (lo < hi) {
    idx = (lo + hi) / 2;
    if      (less(key, base[idx])) hi = idx;
    else if (less(base[idx], key)) lo = idx+1;
    else                              return idx;
  }

  return lo;
}

extern inline void btree_insert(struct btree_node **restrict tree, const size_t order, const void *restrict key, void *restrict value, bool (*less)(const void *, const void *)) {
  register size_t            idx;
  register struct btree_node *tmp;
  register struct btree_node *walk    = *tree;
  register struct btree_node *sibling = NULL;
           struct stack      *stack   = NULL;

  while (walk != NULL) {
    if  ((idx = binsearch(key, walk->keys, walk->nmemb, less)) < walk->nmemb &&
        !(less(key, walk->keys[idx]) || less(walk->keys[idx], key))) { destroy(&stack); return; }
    push(&stack, walk);
    push(&stack, (void *)idx);
    walk = walk->children[idx];
  }

  while (!empty(stack)) {
    idx  = (size_t)pop(&stack);
    walk = pop(&stack);

    if (walk->nmemb < order-1) {
      memcpy(&walk->keys[idx+1], &walk->keys[idx], __SIZEOF_POINTER__*(walk->nmemb-idx));
      memcpy(&walk->values[idx+1], &walk->values[idx], __SIZEOF_POINTER__*(walk->nmemb-idx));
      memcpy(&walk->children[idx+2], &walk->children[idx+1], __SIZEOF_POINTER__*(walk->nmemb++-idx));
      walk->keys[idx]       = key;
      walk->values[idx]     = value;
      walk->children[idx+1] = sibling;
      destroy(&stack);
      return;
    }

    tmp = btree_alloc(order+1);
    memcpy(tmp->keys, walk->keys, __SIZEOF_POINTER__*idx);
    memcpy(&tmp->keys[idx+1], &walk->keys[idx], __SIZEOF_POINTER__*(walk->nmemb-idx));
    memcpy(tmp->values, walk->values, __SIZEOF_POINTER__*idx);
    memcpy(&tmp->values[idx+1], &walk->values[idx], __SIZEOF_POINTER__*(walk->nmemb-idx));
    memcpy(tmp->children, walk->children, __SIZEOF_POINTER__*(idx+1));
    memcpy(&tmp->children[idx+2], &walk->children[idx+1], __SIZEOF_POINTER__*(walk->nmemb-idx));
    tmp->keys[idx]       = key;
    tmp->values[idx]     = value;
    tmp->children[idx+1] = sibling;

    sibling        = btree_alloc(order);
    sibling->nmemb = (order-1)>>1;
    walk->nmemb    = order>>1;
    memcpy(walk->keys, tmp->keys, __SIZEOF_POINTER__*walk->nmemb);
    memcpy(sibling->keys, &tmp->keys[walk->nmemb+1], __SIZEOF_POINTER__*sibling->nmemb);
    memcpy(walk->values, tmp->values, __SIZEOF_POINTER__*walk->nmemb);
    memcpy(sibling->values, &tmp->values[walk->nmemb+1], __SIZEOF_POINTER__*sibling->nmemb);
    memcpy(walk->children, tmp->children, __SIZEOF_POINTER__*(walk->nmemb+1));
    memcpy(sibling->children, &tmp->children[walk->nmemb+1], __SIZEOF_POINTER__*(sibling->nmemb+1));
    key   = tmp->keys[walk->nmemb];
    value = tmp->values[walk->nmemb];
    btree_free(tmp);
  }

  *tree                = btree_alloc(order);
  (*tree)->keys[0]     = key;
  (*tree)->values[0]   = value;
  (*tree)->children[0] = walk;
  (*tree)->children[1] = sibling;
  (*tree)->nmemb       = 1;
}

extern inline void btree_erase(struct btree_node **restrict tree, const size_t order, const void *restrict key, bool (*less)(const void *, const void *)) {
  register size_t            idx;
  register struct btree_node *parent;
  register struct btree_node *sibling;
  register struct btree_node *walk  = *tree;
           struct stack      *stack = NULL;

  while (walk != NULL) {
    if  ((idx = binsearch(key, walk->keys, walk->nmemb, less)) < walk->nmemb &&
        !(less(key, walk->keys[idx]) || less(walk->keys[idx], key))) break;
    push(&stack, walk);
    push(&stack, (void *)idx);
    walk = walk->children[idx];
  }

  if (walk == NULL) { destroy(&stack); return; }

  if (walk->children[idx] != NULL) {
    parent = walk;
    push(&stack, walk);
    push(&stack, (void *)idx);
    for (walk = walk->children[idx]; walk->children[walk->nmemb] != NULL; walk = walk->children[walk->nmemb]) {
      push(&stack, walk);
      push(&stack, (void *)walk->nmemb);
    }
    parent->keys[idx]   = walk->keys[walk->nmemb-1];
    parent->values[idx] = walk->values[walk->nmemb-1];
    idx                 = walk->nmemb-1;
  }

  memcpy(&walk->keys[idx], &walk->keys[idx+1], __SIZEOF_POINTER__*(--walk->nmemb-idx));
  memcpy(&walk->values[idx], &walk->values[idx+1], __SIZEOF_POINTER__*(walk->nmemb-idx));

  while (!empty(stack)) {
    if  ((order-1)>>1 <= walk->nmemb) { destroy(&stack); return; }

    idx     = (size_t)pop(&stack);
    parent  = pop(&stack);
    sibling = idx == 0                                                        ? parent->children[1]
            : idx == parent->nmemb                                            ? parent->children[idx-1]
            : parent->children[idx-1]->nmemb < parent->children[idx+1]->nmemb ? parent->children[idx+1]
                                                                              : parent->children[idx-1];
    if   ((order-1)>>1 < sibling->nmemb) {    /* case of key redistribution */
      if (sibling == parent->children[idx-1]) {
        memcpy(&walk->keys[1], walk->keys, __SIZEOF_POINTER__*walk->nmemb);
        memcpy(&walk->values[1], walk->values, __SIZEOF_POINTER__*walk->nmemb);
        memcpy(&walk->children[1], walk->children, __SIZEOF_POINTER__*++walk->nmemb);
        walk->keys[0]         = parent->keys[idx-1];
        walk->values[0]       = parent->values[idx-1];
        walk->children[0]     = sibling->children[sibling->nmemb];
        parent->keys[idx-1]   = sibling->keys[--sibling->nmemb];
        parent->values[idx-1] = sibling->values[sibling->nmemb];
      } else {
        walk->keys[walk->nmemb]       = parent->keys[idx];
        walk->values[walk->nmemb]     = parent->values[idx];
        walk->children[++walk->nmemb] = sibling->children[0];
        parent->keys[idx]             = sibling->keys[0];
        parent->values[idx]           = sibling->values[0];
        memcpy(sibling->children, &sibling->children[1], __SIZEOF_POINTER__*sibling->nmemb);
        memcpy(sibling->keys, &sibling->keys[1], __SIZEOF_POINTER__*--sibling->nmemb);
        memcpy(sibling->values, &sibling->values[1], __SIZEOF_POINTER__*sibling->nmemb);
      }
      destroy(&stack);
      return;
    }

    if (sibling == parent->children[idx-1]) { /* case of node merge */
      sibling->keys[sibling->nmemb]   = parent->keys[idx-1];
      sibling->values[sibling->nmemb] = parent->values[idx-1];
      memcpy(&sibling->keys[++sibling->nmemb], walk->keys, __SIZEOF_POINTER__*walk->nmemb);
      memcpy(&sibling->values[sibling->nmemb], walk->values, __SIZEOF_POINTER__*walk->nmemb);
      memcpy(&sibling->children[sibling->nmemb], walk->children, __SIZEOF_POINTER__*(walk->nmemb+1));
      memcpy(&parent->keys[idx-1], &parent->keys[idx], __SIZEOF_POINTER__*(parent->nmemb-idx));
      memcpy(&parent->values[idx-1], &parent->values[idx], __SIZEOF_POINTER__*(parent->nmemb-idx));
      memcpy(&parent->children[idx], &parent->children[idx+1], __SIZEOF_POINTER__*(parent->nmemb---idx));
      sibling->nmemb += walk->nmemb;
      btree_free(walk);
    } else {
      walk->keys[walk->nmemb]   = parent->keys[idx];
      walk->values[walk->nmemb] = parent->values[idx];
      memcpy(&walk->keys[++walk->nmemb], sibling->keys, __SIZEOF_POINTER__*sibling->nmemb);
      memcpy(&walk->values[walk->nmemb], sibling->values, __SIZEOF_POINTER__*sibling->nmemb);
      memcpy(&walk->children[walk->nmemb], sibling->children, __SIZEOF_POINTER__*(sibling->nmemb+1));
      memcpy(&parent->keys[idx], &parent->keys[idx+1], __SIZEOF_POINTER__*(--parent->nmemb-idx));
      memcpy(&parent->values[idx], &parent->values[idx+1], __SIZEOF_POINTER__*(parent->nmemb-idx));
      memcpy(&parent->children[idx+1], &parent->children[idx+2], __SIZEOF_POINTER__*(parent->nmemb-idx));
      walk->nmemb += sibling->nmemb;
      btree_free(sibling);
    }
    walk = parent;
  }

  if (walk->nmemb == 0) { *tree = walk->children[0]; btree_free(walk); }
}

extern inline void btree_inorder(const struct btree_node *restrict tree, void (*func)(const void *restrict, void *restrict)) {
  if (tree != NULL) {
    for (size_t idx = 0; idx < tree->nmemb; ++idx) {
      btree_inorder(tree->children[idx], func);
      func(tree->keys[idx], tree->values[idx]);
    }
    btree_inorder(tree->children[tree->nmemb], func);
  }
}

#endif /* _BTREE_H */
