#ifndef BUFFER_H
#define BUFFER_H

/* The char_node and line_node structs represent respectively a character of a sigular line
 * and a line of a singular buffer, in the form of a double linked list.
 */
struct char_node {
  int elem;
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
 * It also contains the position of the cursor in the x-y axis.
 */
struct buffer {
  struct line_node *first_line;
  struct line_node *last_line;
  struct line_node *current_line;
  struct char_node *current_char;
  int cursor_x;
  int cursor_y;
};

// Creates a new empty line
struct line_node *line_node_new();

// Frees the memory of a line_node
void line_node_free(struct line_node *line);

// Creates a new buffer with an empty line
struct buffer *buffer_new();

// Frees the memory of a buffer
void buffer_free(struct buffer *buff);

// Inserts a character into the buffer
void buffer_insert_char(struct buffer *buff, int ch);

// Moves the buffer cursor
void buffer_move_cursor(struct buffer *buff, int dy, int dx);

#endif // BUFFER_H
