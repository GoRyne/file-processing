#include <stdio.h>

#include "bstree.h"

void print(struct bst_node *node) { printf("%d ", node->key); }

int main(void) {
  int testcases[] = {40, 11, 77, 33, 20, 90, 99, 70, 88, 80, 66, 10, 22, 30, 44, 55, 50, 60, 25, 49};

  struct bst_node *tree = NULL;

  for (const int *it = testcases; it < testcases + sizeof(testcases) / sizeof(int); ++it) {
    bst_insert(&tree, *it, 0);
    bst_inorder(tree, print);
    printf("\n");
  }
  for (const int *it = testcases; it < testcases + sizeof(testcases) / sizeof(int); ++it) {
    bst_erase(&tree, *it);
    bst_inorder(tree, print);
    printf("\n");
  }
  for (const int *it = testcases; it < testcases + sizeof(testcases) / sizeof(int); ++it) {
    bst_insert(&tree, *it, 0);
    bst_inorder(tree, print);
    printf("\n");
  }
  for (const int *it = testcases + sizeof(testcases) / sizeof(int) - 1; testcases <= it; --it) {
    bst_erase(&tree, *it);
    bst_inorder(tree, print);
    printf("\n");
  }
}
