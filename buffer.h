#ifndef BUFFER_H
#define BUFFER_H

#include <wchar.h>

/* The char_node and line_node structs represent respectively a character of a sigular line
 * and a line of a singular buffer, in the form of a double linked list.
 */
struct char_node {
  wchar_t elem[2];
  struct char_node *prev_char;
  struct char_node *next_char;
};

struct line_node {
  struct char_node *first_char;
  struct char_node *last_char;
  struct line_node *prev_line;
  struct line_node *next_line;
};

/* The buffer represents a structure of a double linked list containing the lines,
 * and references to the current line and the current char pointed by the cursor.
 * It also contains the position of the cursor in the x-y axis. The variable
 * cursor_real_x is used when we are on a line and we move to a shorter one
 * and the cursor is forced to move to the left. The value of the last position
 * is stored in that variable and remains unchanged unless we move horizontally.
 */
struct buffer {
  struct line_node beg_sentinel; // TODO: make const if possible
  struct line_node end_sentinel;
  struct line_node *current_line;
  struct char_node *current_char;
  int cursor_x;
  int cursor_real_x;
  int cursor_y;
};

#define buffer_first_line(buff) (buff->beg_sentinel.next_line)
#define buffer_last_line(buff) (buff->end_sentinel.prev_line)

#define buffer_move_cursor_x_end(buff) while(buffer_move_cursor_x(buff, 1))
#define buffer_move_cursor_x_home(buff) while(buffer_move_cursor_x(buff, -1))
#define buffer_update_cursor_real_x(buff) (buff->cursor_real_x = buff->cursor_x)

// Creates a new empty line
struct line_node *line_node_new();

// Frees the memory of a line_node
void line_node_free(struct line_node *line);

// Creates a new buffer with an empty line
struct buffer *buffer_new();

// Frees the memory of a buffer
void buffer_free(struct buffer *buff);

// Inserts a character into the buffer
void buffer_insert_char(struct buffer *buff, wchar_t *ch);

// Deletes the character that the cursor is pointing to
void buffer_delete_char(struct buffer *buff);

// Inserts a new line into the buffer
void buffer_insert_line(struct buffer *buff);

// Moves the x coordinate of the buffer cursor.
// Returns the numbers of steps that the cursor has moved
int buffer_move_cursor_x(struct buffer *buff, int dx);

// Moves the y coordinate of the buffer cursor
// Returns the numbers of steps that the cursor has moved
int buffer_move_cursor_y(struct buffer *buff, int dy);

#endif // BUFFER_H
