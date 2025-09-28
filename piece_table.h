#ifndef PIECE_TABLE_H
#define PIECE_TABLE_H

enum Source { ORIGINAL, ADD };

typedef struct {
  int start;
  int length;
  enum Source source;
} Piece;

typedef struct {
  char *original;
  char *add;
  Piece pieces[];
} PieceTable;

void set_original(PieceTable *piece_table, char *text);

#endif