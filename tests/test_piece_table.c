#include <assert.h>
#include <stdio.h>
#include "piece_table.h"

void test_find_piece_at_index_basic() {
  Piece pieces[] = {
      {0, 5, ORIGINAL},
      {0, 1, ADD},
      {5, 5, ORIGINAL},
  };
  PieceTable pt = {.pieces = pieces, .pieces_len = 3};

  size_t idx, off;

  assert(find_piece_at_index(&pt, 0, &idx, &off));
  assert(idx == 0 && off == 0);

  assert(find_piece_at_index(&pt, 5, &idx, &off));
  assert(idx == 1 && off == 0);

  assert(find_piece_at_index(&pt, 6, &idx, &off));
  assert(idx == 2 && off == 0);

  assert(find_piece_at_index(&pt, 11, &idx, &off));
  assert(idx == 2 && off == 5);

  assert(!find_piece_at_index(&pt, 12, &idx, &off));
}

int main() {
  test_find_piece_at_index_basic();
  printf("All tests passed!\n");
  return 0;
}
