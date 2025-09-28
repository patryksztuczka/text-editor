#include "piece_table.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void draw_gutter(WINDOW *win, int *lines) {
  for (int i = 0; i < *lines; i++) {
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

typedef struct {
  int x_pos;
  int y_pos;
  int lines_count;
  char text[100];
  int text_len;
} EditorState;

void handle_functional_keys(WINDOW *win, int *ch, EditorState *state) {
  switch (*ch) {
  case 27: // ESC
    break;

  case 127: // BACKSPACE
    if (state->x_pos > 0) {
      memmove(&state->text[state->x_pos - 1], &state->text[state->x_pos],
              state->text_len - (state->x_pos - 1));

      state->text_len--;
      state->x_pos--;

      wmove(win, state->y_pos, state->x_pos);   // ustawiamy kursor
      wclrtoeol(win);                           // czyścimy resztę linii
      waddnstr(win, &state->text[state->x_pos], // rysujemy od miejsca zmiany
               state->text_len - state->x_pos);
      wmove(win, state->y_pos, state->x_pos); // wracamy kursorem
      wrefresh(win);
    }
    break;
  }
}

// static const EditorState CONFIG_DEFAULT = {
//     .x_pos = 0,
//     .y_pos = 0,
//     .lines_count = 1,
//     .text_len = 0,
// };

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

  FILE *fileptr = fopen(argv[1], "r");

  if (!fileptr) {
    wprintw(textArea, "Nie mogę otworzyć pliku");
    return 1;
  }

  fseek(fileptr, 0, SEEK_END);
  long size = ftell(fileptr);
  rewind(fileptr);

  char *buf = malloc(size + 1);

  if (!buf) {
    fclose(fileptr);
    return 1;
  }

  fread(buf, 1, size, fileptr);
  buf[size] = '\0';

  PieceTable piece_table;
  piece_table.original = NULL;

  // EditorState state = CONFIG_DEFAULT;
  set_original(&piece_table, buf);
  free(buf);

  int ch;

  while (1) {
    wprintw(textArea, "%s ", piece_table.original);
    ch = wgetch(textArea);
    // if (is_arrow_key(&ch)) {
    //   handle_arrow_keys(textArea, &state.y_pos, &state.x_pos, &ch);
    // } else if (is_functional_key(&ch)) {
    //   handle_functional_keys(textArea, &ch, &state);
    // } else {
    //   waddch(textArea, ch);
    //   state.text[state.text_len] = ch;
    //   state.text_len++;
    //   if (ch == 10) {
    //     state.x_pos = 0;
    //     state.y_pos++;
    //     state.lines_count++;
    //     draw_gutter(gutter, &state.lines_count);
    //   } else {
    //     state.x_pos++;
    //   }
    // }
  }

  // FILE *fileptr = fopen("test.txt", "w");

  // fprintf(fileptr, "%s", state.text);

  // fclose(fileptr);

  delwin(textArea);
  delwin(gutter);
  endwin();
  return 0;
}