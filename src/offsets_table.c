#include "offsets_table.h"
#include <stdio.h>

void add_line_offset(OffsetsTable *table, size_t new_offset) {
  size_t new_count = table->lines_count + 1;

  size_t *new_ptr = realloc(table->line_offsets, new_count * sizeof(size_t));

  if (!new_ptr) {
    perror("realloc failed");
    exit(1);
  }

  table->line_offsets = new_ptr;
  table->line_offsets[table->lines_count] = new_offset + 1;
  table->lines_count = new_count;
}
