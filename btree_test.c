#include <stdio.h>

#include "btree.h"

bool less(const void *a, const void *b) { return *(int *)a < *(int *)b; }

void print(const struct btree_node *node) { return; }

int main(void) {
  const int testcases[] = {
    40, 11, 77, 33, 20, 90, 99, 70, 88, 80,
		66, 10, 22, 30, 44, 55, 50, 60, 100, 28,
		18, 9, 5, 17, 6, 3, 1, 4, 2, 7,
		8, 73, 12, 13, 14, 16, 15, 25, 24, 28,
		45, 49, 42, 43, 41, 47, 48, 46, 63, 68,
		61, 62, 64, 69, 67, 65, 54, 59, 58, 51,
		53, 57, 52, 56, 83, 81, 82, 84, 75, 89,
		66, 10, 22, 30, 44, 55, 50, 60, 100, 28,
		18, 9, 5, 17, 6, 3, 1, 4, 2, 7,
		8, 73, 12, 13, 14, 16, 15, 25, 24, 28,
		40, 11, 77, 33, 20, 90, 99, 70, 88, 80,
		45, 49, 42, 43, 41, 47, 48, 46, 63, 68,
		53, 57, 52, 56, 83, 81, 82, 84, 75, 89,
		61, 62, 64, 69, 67, 65, 54, 59, 58, 51,
  };

  struct btree_node *tree = NULL;

  for (const int *it = testcases; it < testcases + sizeof(testcases)/sizeof(int)/2; ++it) {
    btree_insert(&tree, 3, it, NULL, less);
    btree_inorder(tree, print);
    printf("\n");
  }
  for (const int *it = testcases + sizeof(testcases)/sizeof(int)/2; it < testcases + sizeof(testcases)/sizeof(int); ++it) {
    btree_erase(&tree, 3, it, less);
    btree_inorder(tree, print);
    printf("\n");
  }
  for (const int *it = testcases; it < testcases + sizeof(testcases)/sizeof(int)/2; ++it) {
    btree_insert(&tree, 4, it, NULL, less);
    btree_inorder(tree, print);
    printf("\n");
  }
  for (const int *it = testcases + sizeof(testcases)/sizeof(int)/2; it < testcases + sizeof(testcases)/sizeof(int); ++it) {
    btree_erase(&tree, 4, it, less);
    btree_inorder(tree, print);
    printf("\n");
  }
}
