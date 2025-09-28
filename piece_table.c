#include "piece_table.h"
#include <stdlib.h>
#include <string.h>

void set_original(PieceTable *piece_table, char *buf) {
  if (!piece_table)
    return;

  if (piece_table->original)
    free(piece_table->original);

  piece_table->original = malloc(strlen(buf) + 1);
  if (piece_table->original) {
    strcpy(piece_table->original, buf);
  }
}