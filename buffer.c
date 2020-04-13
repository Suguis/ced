#include "buffer.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <iconv.h>
#include <string.h>

struct char_node *char_node_new(unsigned int ch) {
  struct char_node *new = safe_malloc(sizeof(struct char_node));
  new->elem[0] = ch; // Char copy
  new->elem[1] = '\0';
  new->prev_char = new->next_char = NULL;
  return new;
}

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

struct buffer *buffer_new(char *filename) {
  struct buffer *buff = safe_malloc(sizeof(struct buffer));
  
  buffer_first_line(buff) = buffer_last_line(buff) = buff->current_line = line_node_new();
  buff->current_line->prev_line = &buff->beg_sentinel;
  buff->current_line->next_line = &buff->end_sentinel;
  buff->current_char = buff->current_line->first_char;
  buff->cursor_x = 0;
  buff->cursor_real_x = 0;
  buff->cursor_y = 0;
  
  if (filename) {
    buff->name = filename;
    buffer_read_file(buff, filename);
  } else {
    buff->name = "[New file]";
  }
  
  return buff;
}

void buffer_free(struct buffer *buff) {
  struct line_node *line = buffer_first_line(buff);
  while(line != &buff->end_sentinel) {
    struct line_node *next = line->next_line;
    line_node_free(line);
    line = next;
  }
  free(buff);
}

void buffer_insert_char(struct buffer *buff, unsigned int ch) {
  struct char_node *new = char_node_new(ch);
  
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
  if (!buffer_eol(buff)) {
    buff->current_char->next_char->prev_char = buff->current_char->prev_char;
    if (buffer_bol(buff)) {
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

  buff->current_line->next_line->prev_line = new;
  buff->current_line->next_line = new;
}

void buffer_delete_line(struct buffer *buff) {
  if (buffer_first_line(buff) == buffer_last_line(buff)) {
    buffer_move_x_home(buff);
    while (!line_node_empty(buffer_first_line(buff))) {
      buffer_delete_char(buff);
    }
  } else {
    struct line_node *deleted = buff->current_line;
    
    if (buffer_eob(buff)) buffer_move_y(buff, -1);
    else {
      buffer_move_y(buff, 1);
      buff->cursor_y--;
    }
    
    deleted->prev_line->next_line = deleted->next_line;
    deleted->next_line->prev_line = deleted->prev_line;
    line_node_free(deleted);
  }
}

void buffer_split_line(struct buffer *buff) {
  // Create and link the new line
  struct line_node *line = safe_malloc(sizeof(struct line_node));
  line->prev_line = buff->current_line;
  line->next_line = buff->current_line->next_line;
  line->first_char = buff->current_char;
  line->last_char = buff->current_line->last_char;

  // Create and link the new char
  struct char_node *node = char_node_new('\0');
  if (!buffer_bol(buff)) node->prev_char = buff->current_char->prev_char;

  // Modify the previous char links
  if (!buffer_bol(buff)) node->prev_char->next_char = node;
  buff->current_char->prev_char = NULL;

  // Modify the previous line links
  if (buffer_bol(buff)) buff->current_line->first_char = node;
  buff->current_line->last_char = node;
  line->next_line->prev_line = line;
  line->prev_line->next_line = line;

  buff->current_char = node;
}

void buffer_join_line(struct buffer *buff) {
  if (!buffer_eob(buff)) {
    if (line_node_empty(buff->current_line)) {
      buffer_delete_line(buff);
      return;
    }
    int eol = buffer_eol(buff);
    
    struct char_node *last = buff->current_line->last_char->prev_char;
    free(last->next_char);
    last->next_char = buff->current_line->next_line->first_char;
    last->next_char->prev_char = last;
    if (eol) buff->current_char = last->next_char;
    
    buff->current_line->last_char = buff->current_line->next_line->last_char;
    struct line_node *deleted = buff->current_line->next_line;
    buff->current_line->next_line = deleted->next_line;
    if (deleted->next_line) deleted->next_line->prev_line = buff->current_line;
    free(deleted);
  }
}

int buffer_move_x(struct buffer *buff, int dx) {
  // We move the cursor until we reach the bounds or until the requested distance has been moved
  int steps = 0;
  if (dx > 0) {
    while (dx != steps && !buffer_eol(buff)) {
      buff->current_char = buff->current_char->next_char;
      steps++;
      buff->cursor_x++;
    }
  } else {
    while (dx != steps && !buffer_bol(buff)) {
      buff->current_char = buff->current_char->prev_char;
      steps--;
      buff->cursor_x--;
    }
  }

  return steps;
}

int buffer_move_y(struct buffer *buff, int dy) {
  int steps = 0;
  if (dy > 0) {
    while (dy != steps && !buffer_eob(buff)) {
      buff->current_line = buff->current_line->next_line;
      steps++;
      buff->cursor_y++;
    }
  } else {
    while (dy != steps && !buffer_bob(buff)) {
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
  while (x != buff->cursor_real_x && !buffer_eol(buff)) {
    buff->current_char = buff->current_char->next_char;
    x++;
  }
  buff->cursor_x = x;

  return steps;
}

void buffer_read_file(struct buffer *buff, char *filename) {
  FILE *file = fopen(filename, "r");
  // If the file doesn't exist, we start with an empty buffer for it
  if (!file) return;

  // Get file size
  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  // Read the file into the buffer
  char file_buff[file_size];
  memset(file_buff, 0, sizeof(file_buff));
  char *file_ptr = file_buff;
  fread(file_buff, sizeof(char), file_size-1, file); // -1 to delete remove '\n' on file

  // We multiply the size of the file by 4 to get enought memory for the conversion
  size_t text_size = file_size * 4;

  // We create a buffer for the text stored in Unicode
  char text_buff[text_size + 4];
  memset(text_buff, 0, sizeof(text_buff));
  char *text_ptr = text_buff;

  // We convert the file contents to Unicode and store it in text_buff
  iconv_t conversion = iconv_open("Unicode", "UTF-8");
  iconv(conversion, &file_ptr, &file_size, &text_ptr, &text_size);
  iconv_close(conversion);

  // Skip Unicode magic nunmber
  text_ptr = text_buff + 2;

  // We finally put the unicode characters into the text buffer of the editor
  unsigned char *p = (unsigned char*)text_ptr;
  while (1) {
    // Read 16-bit little endian character
    unsigned int ch;
    ch = *p;
    p++;
    ch |= *p << 8;
    p++;
      
    if (ch == '\n') {
      buffer_split_line(buff);
      buffer_move_y(buff, 1);
    } else if (!ch) { // If is a null character, we have finishedx
      break;
    } else {
      buffer_insert_char(buff, ch);
      buffer_move_x(buff, 1);
    }
  }
}
