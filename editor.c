#include "editor.h"
#include "buffer.h"
#include <curses.h>
#include <locale.h>
#include <stddef.h>
#include <stdlib.h>

struct Editor *editor_new() {
  struct Editor *ed = malloc(sizeof(struct Editor));
  if (ed == NULL) exit(-1);
  ed->needs_exit = 0;
  ed->buff = buffer_new();
  
  setlocale(LC_ALL, "");
  initscr();
  raw();
  noecho();
  nonl();
  keypad(stdscr, true);
  // printw("Helloaá World !!!");
  refresh();

  return ed;
}

void editor_exit(struct Editor *ed) {
  struct Buffer *b = buffer_new();
  buffer_free(b);
  buffer_free(ed->buff);
  free(ed);
  endwin();
}

int editor_needs_exit(struct Editor *ed) {
  return ed->needs_exit;
}

int editor_get_key() {
  return getch();
}

void editor_interpret_key(struct Editor *ed, int key) {
  switch (key) {
  case KEY_UP:
    printw("UP");
    break;
  case KEY_DOWN:
    printw("DOWN");
    break;
  case KEY_LEFT:
    printw("LEFT");
    break;
  case KEY_RIGHT:
    printw("RIGHT");
    break;
  case 'q':
    ed->needs_exit = 1;
    break;
  default:
    buffer_insert_char(ed->buff, key);
    buffer_move_cursor(ed->buff, 0, 1);
  }
}

void editor_refresh(struct Editor *ed) {
  int y = 0;
  int x = 0;
  struct LineNode *l_node = ed->buff->first;
  struct CharNode *c_node;
  
  while(l_node != NULL) {
    c_node = l_node->elem->first;
    while(c_node != l_node->elem->last) {
      mvaddch(y, x++, c_node->elem);
      c_node = c_node->next;
    }
    y++;
    l_node = l_node->next;
  }
  move(ed->buff->cursor_y, ed->buff->cursor_x);
  
  refresh();
}
