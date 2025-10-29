#include "piece_table.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_original(PieceTable *piece_table, char *buf) {
  if (!piece_table)
    return;

  if (piece_table->original)
    free(piece_table->original);

  unsigned long buf_size = strlen(buf);

  piece_table->original = malloc(buf_size + 1);
  if (piece_table->original) {
    strcpy(piece_table->original, buf);
  }

  if (piece_table->pieces)
    free(piece_table->pieces);

  Piece new_original_piece = {0, buf_size, ORIGINAL};
  piece_table->pieces = malloc(sizeof(new_original_piece));
  if (piece_table->pieces) {
    *piece_table->pieces = new_original_piece;
  }
  piece_table->pieces_len++;
}

bool find_piece_at_index(PieceTable *pt, size_t idx, size_t *piece_idx,
                         size_t *offset_in_piece) {
  size_t cumulative_length = 0;

  for (size_t i = 0; i < pt->pieces_len; i++) {
    Piece *p = &pt->pieces[i];

    if (idx < cumulative_length + p->length) {
      *piece_idx = i;
      *offset_in_piece = idx - cumulative_length;
      return true;
    }
    cumulative_length += p->length;
  }

  if (idx == cumulative_length) {
    *piece_idx = pt->pieces_len - 1;
    *offset_in_piece = pt->pieces[pt->pieces_len - 1].length;
    return true;
  }

  return false;
}

void split_piece(const Piece *target, size_t offset, Piece *left_out,
                 Piece *right_out) {
  if (offset == 0) {
    *left_out = (Piece){0};
    *right_out = *target;
  } else if (offset >= target->length) {
    *left_out = *target;
    *right_out = (Piece){0};
  } else {
    left_out->start = target->start;
    left_out->length = offset;
    left_out->source = target->source;

    right_out->start = target->start + offset;
    right_out->length = target->length - offset;
    right_out->source = target->source;
  }
}

size_t append_to_add_buffer(PieceTable *pt, const char *s, size_t s_len) {
  if (s_len + pt->add_len + 1 > SIZE_MAX) {
    return (size_t)-1;
  }

  size_t start = pt->add_len;
  size_t new_len = pt->add_len + s_len + 1;
  char *new_add_ptr = realloc(pt->add, new_len);
  if (!new_add_ptr) {
    return (size_t)-1;
  }
  pt->add = new_add_ptr;
  memcpy(new_add_ptr + pt->add_len, s, s_len);
  pt->add_len += s_len;
  pt->add[pt->add_len] = '\0';
  return start;
}

bool insert_piece(PieceTable *pt, size_t target_idx, Piece *left, Piece *mid,
                  Piece *right) {
  size_t new_pieces_count = 0;
  Piece pieces[] = {*left, *mid, *right};
  for (int i = 0; i < 3; i++) {
    if (pieces[i].length)
      new_pieces_count++;
  }

  size_t old_len = pt->pieces_len;
  size_t new_len = new_pieces_count - 1 + old_len;
  Piece *new_pieces_ptr = realloc(pt->pieces, new_len * sizeof(Piece));
  if (!new_pieces_ptr) {
    return false;
  }

  pt->pieces = new_pieces_ptr;
  size_t move_count = old_len - target_idx - 1;
  if (move_count > 0) {
    memmove(&pt->pieces[target_idx + new_pieces_count],
            &pt->pieces[target_idx + 1], move_count * sizeof(Piece));
  }

  size_t pos = target_idx;
  if (left->length)
    pt->pieces[pos++] = *left;
  pt->pieces[pos++] = *mid;
  if (right->length)
    pt->pieces[pos++] = *right;

  pt->pieces_len = new_len;
  return true;
}

bool merge_adjacent_pieces(PieceTable *pt) {
  if (pt->pieces_len < 2) {
    return false;
  }
  size_t write_idx = 0;
  for (size_t i = 1; i < pt->pieces_len; i++) {
    Piece *prev = &pt->pieces[write_idx];
    Piece *curr = &pt->pieces[i];

    if (prev->source == curr->source && prev->source == ADD &&
        curr->source == ADD && prev->start + prev->length == curr->start) {
      prev->length += curr->length;
    } else {
      write_idx++;

      if (write_idx != i) {
        pt->pieces[write_idx] = *curr;
      }
    }
  }
  pt->pieces_len = write_idx + 1;
  return true;
}

bool insert(PieceTable *pt, size_t idx, char *s) {
  if (!pt || !s || strlen(s) == 0) {
    return false;
  }

  size_t s_len = strlen(s);

  if (pt->pieces_len == 0) {
    if (idx != 0)
      return false;
    size_t start = append_to_add_buffer(pt, s, s_len);
    Piece mid = (Piece){start, s_len, ADD};
    Piece *arr = realloc(pt->pieces, sizeof(Piece));
    if (!arr)
      return false;
    pt->pieces = arr;
    pt->pieces[0] = mid;
    pt->pieces_len = 1;
    return true;
  }

  size_t piece_idx, offset_in_piece;
  if (!find_piece_at_index(pt, idx, &piece_idx, &offset_in_piece)) {
    return false;
  }

  Piece *target = &pt->pieces[piece_idx];
  Piece left, right;
  split_piece(target, offset_in_piece, &left, &right);

  size_t start = append_to_add_buffer(pt, s, s_len);

  Piece mid = {start, s_len, ADD};
  if (!insert_piece(pt, piece_idx, &left, &mid, &right)) {
    return false;
  }

  if (!merge_adjacent_pieces(pt)) {
    return false;
  }

  return true;
}

bool find_pieces_in_range(PieceTable *pt, size_t idx, size_t length,
                          size_t **indexes_ptr, size_t *count) {
  size_t cumulative_length = 0;

  for (size_t i = 0; i < pt->pieces_len; i++) {
    Piece *p = &pt->pieces[i];

    if (idx < cumulative_length + p->length) {
      size_t *tmp = realloc(*indexes_ptr, (*count + 1) * sizeof(size_t));
      if (!tmp) {
        return false;
      }
      *indexes_ptr = tmp;
      (*indexes_ptr)[*count] = i;
      *count += 1;
      if (cumulative_length + p->length >= length)
        return true;
    }
    cumulative_length += p->length;
  }

  return false;
}

bool delete(PieceTable *pt, size_t idx, size_t length) {
  if (!pt || length <= 0) {
    return false;
  }

  size_t *pieces_indexes = NULL;
  size_t pieces_count = 0;
  if (!find_pieces_in_range(pt, idx, length, &pieces_indexes, &pieces_count)) {
    return false;
  }

  size_t to_delete = length;
  size_t cumulative_len = 0;

  for (size_t i = 0; i < pieces_count; i++) {
    Piece left, right;
    size_t target_idx = pieces_indexes[i];
    Piece target = pt->pieces[target_idx];
    size_t offset_in_piece = idx - cumulative_len;
    size_t available_to_del_in_piece = target.length - offset_in_piece;
    size_t to_del_in_piece = available_to_del_in_piece > to_delete
                                 ? to_delete
                                 : available_to_del_in_piece;

    if (offset_in_piece == 0 && to_del_in_piece == target.length) {
      left = (Piece){0};
      right = (Piece){0};
    } else if (offset_in_piece == 0 && to_del_in_piece < target.length) {
      left = (Piece){0};
      right.start = offset_in_piece + to_del_in_piece;
      right.length = target.length - to_del_in_piece;
      right.source = target.source;
    } else if (offset_in_piece + to_del_in_piece < target.length) {
      left.start = target.start;
      left.length = offset_in_piece;
      left.source = target.source;
      right.start = offset_in_piece + to_del_in_piece;
      right.length = target.length - right.length - to_del_in_piece;
      right.source = target.source;
    } else {
      left.start = target.start;
      left.length = target.length - to_del_in_piece;
      left.source = target.source;
      right = (Piece){0};
    }

    to_delete -= to_del_in_piece;
    idx += to_del_in_piece;

    if (!insert_piece(pt, target_idx, &left, &(Piece){0}, &right)) {
      return false;
    }

    if (!merge_adjacent_pieces(pt)) {
      return false;
    }
  }

  return true;
}

char *read_buffer(PieceTable *pt) {
  if (!pt || pt->pieces_len == 0) {
    // pusty dokument
    char *empty = malloc(1);
    if (empty)
      empty[0] = '\0';
    return empty;
  }

  // 1) policz łączną długość
  size_t total = 0;
  for (size_t i = 0; i < pt->pieces_len; ++i) {
    total += (size_t)pt->pieces[i].length;
  }

  // 2) alokuj wynik + NUL
  char *buf = malloc(total + 1);
  if (!buf)
    return NULL;

  // 3) kopiuj kawałki po kolei
  size_t off = 0;
  for (size_t i = 0; i < pt->pieces_len; ++i) {
    Piece p = pt->pieces[i];
    const char *src = (p.source == ORIGINAL) ? pt->original : pt->add;

    // (opcjonalnie) można dodać asercje sprawdzające zakres:
    // assert(src && (size_t)p.start + (size_t)p.length <= strlen(src));

    memcpy(buf + off, src + p.start, (size_t)p.length);
    off += (size_t)p.length;
  }

  // 4) zakończ NUL-em
  buf[off] = '\0';
  return buf; // caller: free(buf)
}
