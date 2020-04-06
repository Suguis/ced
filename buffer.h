#ifndef BUFFER_H
#define BUFFER_H

struct CharNode {
  int elem;
  struct CharNode *prev;
  struct CharNode *next;
};

struct Line {
  struct CharNode *first;
  struct CharNode *last;
  struct CharNode *empty; // Used to represent the most right avaliable position of the cursor
};
// TODO: Delete Line structure and put elements into LineNode
struct LineNode {
  struct Line *elem;
  struct LineNode *prev;
  struct LineNode *next;
};

struct Buffer {
  struct LineNode *first;
  struct LineNode *last;
  struct LineNode *current_line;
  struct CharNode *current_char;
  int cursor_x;
  int cursor_y;
};

struct Buffer *buffer_new();
void buffer_free(struct Buffer *buff);
void buffer_insert_char(struct Buffer *buff, int ch);
void buffer_move_cursor(struct Buffer *buff, int dy, int dx);

#endif // BUFFER_H
