#include "offsets_table.h"
#include "piece_table.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int g_should_run = 1;

void draw_gutter(WINDOW *win, size_t lines) {
  for (int i = 0; i <= lines; i++) {
    wmove(win, i, 0);
    wprintw(win, "%d", i + 1);
  }
  wnoutrefresh(win);
  doupdate();
}

int is_arrow_key(int *ch) {
  return *ch == KEY_UP || *ch == KEY_DOWN || *ch == KEY_LEFT ||
         *ch == KEY_RIGHT;
}

int is_functional_key(int *ch) { return *ch == 27 || *ch == 127; }

void handle_arrow_keys(WINDOW *win, int *y, int *x, int *ch) {
  switch (*ch) {
  case KEY_UP:
    (*y)--;
    break;
  case KEY_DOWN:
    (*y)++;
    break;
  case KEY_LEFT:
    (*x)--;
    break;
  case KEY_RIGHT:
    (*x)++;
    break;
  }
  wmove(win, *y, *x);
}

void handle_functional_keys(WINDOW *win, int *ch) {
  switch (*ch) {
  case 27: // ESC
    g_should_run = 0;
    break;
  case 127: // BACKSPACE
    break;
  }
}

static const OffsetsTable OFFSETS_DEFAULT = {.line_offsets = NULL,
                                             .lines_count = 0};

int main(int argc, char **argv) {
  initscr();
  start_color();
  cbreak();
  noecho();
  curs_set(1); // what's this??
  use_default_colors();

  init_pair(1, COLOR_RED, COLOR_WHITE);

  int gutter_w = 2;
  WINDOW *gutter = newwin(0, gutter_w, 0, 0);
  WINDOW *textArea = newwin(0, 0, 0, gutter_w);

  wbkgd(gutter, COLOR_PAIR(1));

  keypad(textArea, TRUE);
  wmove(textArea, 0, 0);
  waddch(gutter, '1');

  wnoutrefresh(gutter);
  wnoutrefresh(textArea);
  doupdate();

  OffsetsTable offsets = OFFSETS_DEFAULT;

  PieceTable piece_table;
  piece_table.original = NULL;
  piece_table.original_len = 0;
  piece_table.add = NULL;
  piece_table.add_len = 0;
  piece_table.pieces = NULL;
  piece_table.pieces_len = 0;

  // if (argv[1]) {
  //   FILE *fileptr = fopen(argv[1], "r");

  //   if (!fileptr) {
  //     wprintw(textArea, "Nie mogę otworzyć pliku");
  //     return 1;
  //   }

  //   fseek(fileptr, 0, SEEK_END);
  //   long size = ftell(fileptr);
  //   rewind(fileptr);

  //   char *buf = malloc(size + 1);

  //   if (!buf) {
  //     fclose(fileptr);
  //     return 1;
  //   }

  //   fread(buf, 1, size, fileptr);
  //   buf[size] = '\0';

  //   set_original(&piece_table, buf);
  //   print_pieces(textArea, &piece_table);
  //   free(buf);

  //   insert(&piece_table, 20, "went to the park and\n");
  //   print_pieces(textArea, &piece_table);
  //   wprintw(textArea, "%s ", piece_table.original);
  // }

  int ch;
  int cy, cx;

  while (g_should_run) {

    ch = wgetch(textArea);
    getyx(textArea, cy, cx);

    if (is_arrow_key(&ch)) {
      handle_arrow_keys(textArea, &cy, &cx, &ch);
    } else if (is_functional_key(&ch)) {
      handle_functional_keys(textArea, &ch);
    } else {
      char s[2] = {(char)ch, '\0'};
      int offsets_sum = 0;
      if (offsets.lines_count > 0) {
        for (size_t i = 0; i < cy; i++) {
          offsets_sum += offsets.line_offsets[i];
        }
      }
      insert(&piece_table, cx + offsets_sum, s);
      winsch(textArea, ch);
      if (ch == 10) {
        add_line_offset(&offsets, cx);
        draw_gutter(gutter, offsets.lines_count);
        wmove(textArea, cy + 1, 0);
      } else {
        wmove(textArea, cy, cx + 1);
      }
    }
  }

  FILE *fileptr = fopen("test.txt", "wb");

  fprintf(fileptr, "%s", read_buffer(&piece_table));

  fclose(fileptr);

  delwin(textArea);
  delwin(gutter);
  endwin();
  return 0;
}
