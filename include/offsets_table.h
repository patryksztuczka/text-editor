#ifndef OFFSETS_TABLE_H
#define OFFSETS_TABLE_H

#include <stdlib.h>

typedef struct {
  size_t *line_offsets;
  size_t lines_count;
} OffsetsTable;

void add_line_offset(OffsetsTable *ot, size_t new_offset);
size_t get_offset(OffsetsTable const *ot, size_t y);

#endif
