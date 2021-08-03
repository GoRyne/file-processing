#include <stdio.h>

#include "rbtree.h"

void print(const struct rb_node *restrict node) { printf(node->color == BLACK ? "\033[01;30m%d\033[00m " : "\033[01;31m%d\033[00m ", node->key); }

int main(void) {
  int testcases[] = {40, 11, 77, 33, 20, 90, 99, 70, 88, 80, 66, 10, 22, 30, 44, 55, 50, 60, 25, 49};

  struct rb_node *tree = NULL;

  for (const int *it = testcases; it < testcases + sizeof(testcases) / sizeof(int); ++it) {
    rb_insert(&tree, *it, 0);
    rb_inorder(tree, print);
    printf("\n");
  }
  for (const int *it = testcases; it < testcases + sizeof(testcases) / sizeof(int); ++it) {
    rb_erase(&tree, *it);
    rb_inorder(tree, print);
    printf("\n");
  }
  for (const int *it = testcases; it < testcases + sizeof(testcases) / sizeof(int); ++it) {
    rb_insert(&tree, *it, 0);
    rb_inorder(tree, print);
    printf("\n");
  }
  for (const int *it = testcases + sizeof(testcases)/sizeof(int)-1; testcases <= it; --it) {
    rb_erase(&tree, *it);
    rb_inorder(tree, print);
    printf("\n");
  }
}
