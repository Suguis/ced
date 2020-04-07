#include "editor.h"
#include "buffer.h"
#include "util.h"
#include <curses.h>
#include <locale.h>
#include <stddef.h>
#include <stdlib.h>

struct editor *editor_new() {
  struct editor *ed = safe_malloc(sizeof(struct editor));
  ed->needs_exit = 0;
  ed->buff = buffer_new();
  
  setlocale(LC_ALL, "");
  initscr();
  raw();
  noecho();
  nonl();
  keypad(stdscr, true);
  refresh();

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

int editor_get_key() {
  return getch();
}

void editor_interpret_key(struct editor *ed, int key) {
  switch (key) {
  case KEY_UP:
    break;
  case KEY_DOWN:
    break;
  case KEY_LEFT:
    buffer_move_cursor(ed->buff, 0, -1);
    break;
  case KEY_RIGHT:
    buffer_move_cursor(ed->buff, 0, 1);
    break;
  case 'q':
    ed->needs_exit = 1;
    break;
  default:
    buffer_insert_char(ed->buff, key);
    buffer_move_cursor(ed->buff, 0, 1);
  }
  //mvaddch(12,1,ed->buff->current_char->elem);
}

void editor_refresh(struct editor *ed) {
  int y = 0;
  int x = 0;
  struct line_node *line = ed->buff->first_line;
  struct char_node *node;
  
  while(line != NULL) {
    node = line->first_char;
    while(node != line->last_char) {
      mvaddch(y, x++, node->elem);
      node = node->next_char;
    }
    y++;
    line = line->next_line;
  }
  move(ed->buff->cursor_y, ed->buff->cursor_x);
  
  refresh();
}
