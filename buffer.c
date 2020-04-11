#include "buffer.h"
#include "util.h"
#include <stdlib.h>

struct line_node *line_node_new() {
  struct line_node *line = safe_malloc(sizeof(struct line_node));
  
  // We create an empty char_node, to make it possible to point one character to the
  // right from the last character of the line.
  line->first_char = line->last_char = safe_malloc(sizeof(struct char_node));
  line->first_char->prev_char = line->first_char->next_char = NULL;
  
  return line;
}

void line_node_free(struct line_node *line) {
  struct char_node *node = line->first_char;
  while(node != NULL) {
    struct char_node *next = node->next_char;
    free(node);
    node = next;
  }
  free(line);
}

struct buffer *buffer_new() {
  struct buffer *buff = safe_malloc(sizeof(struct buffer));
  
  buff->first_line = buff->last_line = buff->current_line = line_node_new();
  buff->current_line->prev_line = buff->current_line->next_line = NULL;
  buff->current_char = buff->current_line->first_char;
  buff->cursor_x = 0;
  buff->cursor_real_x = 0;
  buff->cursor_y = 0;
  
  return buff;
}

void buffer_free(struct buffer *buff) {
  struct line_node *line = buff->first_line;
  while(line != NULL) {
    struct line_node *next = line->next_line;
    line_node_free(line);
    line = next;
  }
  free(buff);
}

void buffer_insert_char(struct buffer *buff, wchar_t *ch) {
  struct char_node *new = safe_malloc(sizeof(struct char_node));
  new->elem[0] = ch[0]; // Char copy
  new->elem[1] = ch[1];

  struct char_node *new_prev = buff->current_char->prev_char;
  struct char_node *new_next = buff->current_char;
  new->prev_char = new_prev;
  new->next_char = new_next;
  
  if (new_prev == NULL) buff->current_line->first_char = new;
  else new_prev->next_char = new;

  buff->current_char = new;
  
  new_next->prev_char = new;
}

void buffer_delete_char(struct buffer *buff) {
  if (buff->current_char != buff->current_line->last_char) {
    buff->current_char->next_char->prev_char = buff->current_char->prev_char;
    if (buff->current_char == buff->current_line->first_char) {
      buff->current_line->first_char = buff->current_char->next_char;
    } else {
      buff->current_char->prev_char->next_char = buff->current_char->next_char;
    }
    struct char_node *deleted = buff->current_char;
    buff->current_char = buff->current_char->next_char;
    free(deleted);
  }
}

void buffer_insert_line(struct buffer *buff) {
  struct line_node *new = line_node_new();
  new->prev_line = buff->current_line;
  new->next_line = buff->current_line->next_line;

  if (buff->current_line->next_line) buff->current_line->next_line->prev_line = new;
  buff->current_line->next_line = new;
}

int buffer_move_cursor_x(struct buffer *buff, int dx) {
  // We move the cursor until we reach the bounds or until the requested distance has been moved
  int steps = 0;
  if (dx > 0) {
    while (dx != steps && buff->current_char->next_char != NULL) {
      buff->current_char = buff->current_char->next_char;
      steps++;
      buff->cursor_x++;
    }
  } else {
    while (dx != steps && buff->current_char->prev_char != NULL) {
      buff->current_char = buff->current_char->prev_char;
      steps--;
      buff->cursor_x--;
    }
  }
  buff->cursor_real_x = buff->cursor_x;

  return steps;
}

int buffer_move_cursor_y(struct buffer *buff, int dy) {
  int steps = 0;
  if (dy > 0) {
    while (dy != steps && buff->current_line->next_line != NULL) {
      buff->current_line = buff->current_line->next_line;
      steps++;
      buff->cursor_y++;
    }
  } else {
    while (dy != steps && buff->current_line->prev_line != NULL) {
      buff->current_line = buff->current_line->prev_line;
      steps--;
      buff->cursor_y--;
    }
  }
  
  /* We need to move again the cursor to the same x position of the previous line,
   * or the maximum possible, if the actual line is shorter than the previous line
   */
  buff->current_char = buff->current_line->first_char;
  int x = 0;
  while (x != buff->cursor_real_x && buff->current_char != buff->current_line->last_char) {
    buff->current_char = buff->current_char->next_char;
    x++;
  }
  buff->cursor_x = x;

  return steps;
}
