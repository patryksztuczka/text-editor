#include "piece_table.h"
#include <curses.h>
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

void insert(PieceTable *pt, size_t idx, char *s) {
  if (!pt)
    return;

  size_t old_add_len = pt->add ? strlen(pt->add) : 0;
  size_t s_len = strlen(s);

  if (s_len == 0)
    return;

  char *new_add_ptr = realloc(pt->add, old_add_len + s_len + 1);
  memcpy(new_add_ptr + old_add_len, s, s_len);
  new_add_ptr[old_add_len + s_len] = '\0';
  pt->add = new_add_ptr;

  // modify "pieces"
  Piece new_piece = {old_add_len, s_len, 1};

  if (pt->pieces_len == 0) {
    if (pt->pieces) {
      free(pt->pieces);
    }
    Piece *new_pieces_ptr = malloc(sizeof(Piece));
    memcpy(new_pieces_ptr, &new_piece, sizeof new_piece);
    pt->pieces = new_pieces_ptr;
    pt->pieces_len += 1;
    return;
  }

  size_t pieces_sum = 0;
  size_t prev_pieces_sum = 0;

  for (size_t i = 0; i < pt->pieces_len; i++) {
    if (i > 0) {
      prev_pieces_sum = pieces_sum;
    }
    pieces_sum += pt->pieces[i].length;
    if (idx == pieces_sum) {
      Piece *new_pieces_ptr =
          realloc(pt->pieces, (pt->pieces_len + 1) * sizeof(Piece));
      size_t pieces_to_shift = pt->pieces_len - (i + 1);
      memmove(new_pieces_ptr + i + 2, new_pieces_ptr + i + 1,
              pieces_to_shift * sizeof(Piece));
      new_pieces_ptr[i + 1] = new_piece;
      pt->pieces = new_pieces_ptr;
      pt->pieces_len += 1;
      break;
    } else if (idx > prev_pieces_sum && idx < pieces_sum) {
      // handle piece splinting
    }
  }
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

void print_pieces(WINDOW *win, PieceTable *piece_table) {
  for (size_t i = 0; i < piece_table->pieces_len; i++) {
    Piece piece = piece_table->pieces[i];
    wprintw(win, "\nStart: %d; Length: %d; Source: %d", piece.start,
            piece.length, piece.source);
  }
}
