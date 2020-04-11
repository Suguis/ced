#define _XOPEN_SOURCE_EXTENDED // To use get_wch()
#include "editor.h"
#include "buffer.h"
#include "util.h"
#include <curses.h>
#include <stddef.h>
#include <stdlib.h>
#include <locale.h>

struct editor *editor_new() {
  struct editor *ed = safe_malloc(sizeof(struct editor));
  ed->needs_exit = 0;
  ed->buff = buffer_new();
  
  setlocale(LC_ALL, "");
  initscr();
  raw();
  noecho();
  set_escdelay(ESC_DELAY);
  nonl();
  keypad(stdscr, true);

  ed->colors_enabled = has_colors();
  
  if (ed->colors_enabled) {
    start_color();
    init_color(COLOR_WHITE, 1000, 1000, 1000);
    init_color(COLOR_BLACK, 0, 0, 0);
    init_pair(TEXT_PAIR, COLOR_WHITE, COLOR_BLACK); 
    init_pair(STATUS_PAIR, COLOR_BLACK, COLOR_WHITE);
    bkgd(COLOR_PAIR(TEXT_PAIR));
  }
  editor_refresh(ed);

  return ed;
}

void editor_exit(struct editor *ed) {
  struct buffer *b = buffer_new();
  buffer_free(b);
  buffer_free(ed->buff);
  free(ed);
  endwin();
}

int editor_needs_exit(struct editor *ed) {
  return ed->needs_exit;
}

unsigned int editor_get_key() {
  unsigned int key;
  get_wch(&key);
  return key;
}

void editor_interpret_key(struct editor *ed, unsigned int key) {
  wchar_t ch[2] = {key, L'\0'};
  
  switch (key) {
  case KEY_UP:
    buffer_move_cursor_y(ed->buff, -1);
    break;
  case KEY_DOWN:
    buffer_move_cursor_y(ed->buff, 1);
    break;
  case KEY_LEFT:
    if (buffer_move_cursor_x(ed->buff, -1)) {
      buffer_update_cursor_real_x(ed->buff);
    }
    break;
  case KEY_RIGHT:
    if (buffer_move_cursor_x(ed->buff, 1)) {
      buffer_update_cursor_real_x(ed->buff);
    }
    break;
  case KEY_ESC:
    ed->needs_exit = 1;
    break;
  case KEY_HOME:
    buffer_move_cursor_x_home(ed->buff);
    break;
  case KEY_END:
    buffer_move_cursor_x_end(ed->buff);
    break;
  case KEY_BACKSPACE:
    // Don't delete the char if we are on the beginning of the line
    if (buffer_move_cursor_x(ed->buff, -1))
      buffer_delete_char(ed->buff);
    break;
  case KEY_ENTER:
  case '\r':
    buffer_insert_line(ed->buff);
    buffer_move_cursor_y(ed->buff, 1);
    buffer_update_cursor_real_x(ed->buff);
    break;
  default:
    buffer_insert_char(ed->buff, ch);
    buffer_move_cursor_x(ed->buff, 1);
  }
}

void editor_refresh(struct editor *ed) {
  clear();
  editor_draw_text(ed);
  editor_draw_status_line(ed);
  move(ed->buff->cursor_y, ed->buff->cursor_x);
  refresh();
}

void editor_draw_text(struct editor *ed) {
  if (ed->colors_enabled) attron(COLOR_PAIR(TEXT_PAIR));
  
  int y = 0;
  int x;
  struct line_node *line = ed->buff->first_line;
  struct char_node *node;
  
  while (line != NULL) {
    x = 0;
    node = line->first_char;
    while(node != line->last_char) {
      mvaddwstr(y, x++, node->elem);
      node = node->next_char;
    }
    y++;
    line = line->next_line;
  }
  
  if (ed->colors_enabled) attroff(COLOR_PAIR(TEXT_PAIR));
}

void editor_draw_status_line(struct editor *ed) {
  if (ed->colors_enabled) attron(COLOR_PAIR(STATUS_PAIR));

  // Draw the name of the file
  mvprintw(LINES - 1, 0, "[New file]");

  // Fill the rest of the line with white spaces
  int y, x;
  getyx(stdscr, y, x);
  while(x++ < COLS) addch(' ');

  // Draw the number of line and column
  mvprintw(LINES - 1, COLS - 8,"%d:%d", ed->buff->cursor_y + 1, ed->buff->cursor_x + 1);
  
  if (ed->colors_enabled) attroff(COLOR_PAIR(STATUS_PAIR));
}
