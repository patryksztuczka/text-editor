#include "piece_table.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void pt_init_empty(PieceTable *pt) { memset(pt, 0, sizeof(PieceTable)); }

static size_t total_len(const PieceTable *pt) {
  size_t sum = 0;
  for (size_t i = 0; i < pt->pieces_len; i++) {
    sum += pt->pieces[i].length;
  }
  return sum;
}

static void assert_piece(PieceTable *pt, size_t idx, size_t start, size_t len,
                         enum Source src) {
  assert(pt->pieces[idx].start == start);
  assert(pt->pieces[idx].length == len);
  assert(pt->pieces[idx].source == src);
}

static void test_insert_empty_creates_add_piece(void) {
  PieceTable pt;
  pt_init_empty(&pt);
  bool ok = insert(&pt, 0, "Hello");
  assert(ok);
  assert_piece(&pt, 0, 0, 5, ADD);
  assert(pt.add_len == 5);
  assert(strcmp(pt.add, "Hello") == 0);
  assert(total_len(&pt) == 5);
}

static void test_insert_beginning_splits_left(void) {
  PieceTable pt;
  pt_init_empty(&pt);
  assert(insert(&pt, 0, "World"));
  assert(insert(&pt, 0, "Hello "));
  print_pieces(&pt);
  assert_piece(&pt, 0, 5, 6, ADD);
  assert_piece(&pt, 1, 0, 5, ADD);
}

int main(void) {
  test_insert_empty_creates_add_piece();
  test_insert_beginning_splits_left();
  printf("All insert tests passed!");
  return 0;
}
