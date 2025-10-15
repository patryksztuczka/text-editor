#ifndef OFFSETS_TABLE_H
#define OFFSETS_TABLE_H

#include <stdlib.h>

typedef struct {
  size_t *line_offsets;
  size_t lines_count;
} OffsetsTable;

void add_line_offset(OffsetsTable *table, size_t new_offset);

#endif