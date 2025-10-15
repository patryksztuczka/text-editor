#ifndef PIECE_TABLE_H
#define PIECE_TABLE_H

#include <curses.h>

enum Source { ORIGINAL, ADD };

typedef struct {
  size_t start;
  size_t length;
  enum Source source;
} Piece;

typedef struct {
  char *original;
  size_t original_len;
  char *add;
  size_t add_len;
  Piece *pieces;
  size_t pieces_len;
} PieceTable;

void set_original(PieceTable *piece_table, char *buf);
void create_piece(char *buf);
bool find_piece_at_index(PieceTable *pt, size_t idx, size_t *piece_idx,
                         size_t *offset_in_piece);
void split_piece(const Piece *target, size_t offset, Piece *left_out,
                 Piece *right_out);
size_t append_to_add_buffer(PieceTable *pt, const char *s, size_t s_len);
bool insert_piece(PieceTable *pt, size_t target_idx, Piece *left, Piece *mid,
                  Piece *right);
bool merge_adjacent_pieces(PieceTable *pt);
bool insert(PieceTable *piece_table, size_t idx, char *s);
char *read_buffer(PieceTable *piece_table);
void print_pieces(const PieceTable *pt);

#endif
