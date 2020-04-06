#include "buffer.h"
#include <stdlib.h>

struct Line *line_new() {
  struct Line *line = malloc(sizeof(struct Line));
  line->first = line->last = line->empty = malloc(sizeof(struct CharNode));
  line->empty->elem = '\0';
  line->empty->prev = line->empty->next = NULL;
  
  return line;
}

void line_free(struct Line *line) {
  struct CharNode *node = line->first;
  while(node != NULL) {
    struct CharNode *next = node->next;
    free(node);
    node = next;
  }
  free(line);
}

struct Buffer *buffer_new() {
  struct Buffer *buff = malloc(sizeof(struct Buffer));
  
  buff->first = buff->last = buff->current_line = malloc(sizeof(struct LineNode));
  buff->current_line->prev = buff->current_line->next = NULL;
  buff->current_line->elem = line_new();
  buff->current_char = buff->current_line->elem->empty;
  buff->cursor_x = 0;
  buff->cursor_y = 0;
  
  return buff;
}

void buffer_free(struct Buffer *buff) {
  struct LineNode *node = buff->first;
  while(node != NULL) {
    struct LineNode *next = node->next;
    line_free(node->elem);
    free(node);
    node = next;
  }
  free(buff);
}

void buffer_insert_char(struct Buffer *buff, int ch) {
  struct CharNode *new = malloc(sizeof(struct CharNode));
  if (new == NULL) exit(-1);
  new->elem = ch;

  struct CharNode *new_prev = buff->current_char->prev;
  struct CharNode *new_next = buff->current_char;
  new->prev = new_prev;
  new->next = new_next;
  
  if (new_prev == NULL) buff->current_line->elem->first = new;
  else new_prev->next = new;

  buff->current_char = new;
  
  new_next->prev = new;
}

void buffer_move_cursor(struct Buffer *buff, int dy, int dx) {
  buff->cursor_x += dx;
  if (dx > 0) {
    while (dx != 0) {
      buff->current_char = buff->current_char->next;
      dx--;
    }
  } else {
    while (dx != 0) {
      buff->current_char = buff->current_char->prev;
      dx++;
    }
  }
  // TODO: Move along lines in the y axis
  buff->cursor_y += dy;
}
