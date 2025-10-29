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

static void test_find_pieces_in_range(void) {
  PieceTable pt;
  pt_init_empty(&pt);
  insert(&pt, 0, "Ala");
  insert(&pt, 3, "kota");
  insert(&pt, 3, " ma ");
  assert(total_len(&pt) == 11);

  size_t *pieces_indexes = NULL;
  size_t pieces_count = 0;
  find_pieces_in_range(&pt, 2, 5, &pieces_indexes, &pieces_count);
  assert(pieces_indexes[0] == 0 && pieces_indexes[1] == 1);
  assert(pieces_count == 2);
}

int main(void) {
  test_find_pieces_in_range();
  printf("All insert tests passed!");
  return 0;
}
