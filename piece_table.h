#ifndef PIECE_TABLE_H
#define PIECE_TABLE_H

#include <curses.h>

enum Source { ORIGINAL, ADD };

typedef struct {
  int start;
  int length;
  enum Source source;
} Piece;

typedef struct {
  char *original;
  char *add;
  Piece *pieces;
  size_t pieces_len;
} PieceTable;

void set_original(PieceTable *piece_table, char *buf);
void create_piece(char *buf);
void insert(PieceTable *piece_table, size_t idx, char *buf);
char *read_buffer(PieceTable *piece_table);
void print_pieces(WINDOW *win, PieceTable *piece_table);

#endif