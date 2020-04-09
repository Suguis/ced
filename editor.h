#ifndef EDITOR_H
#define EDITOR_H

struct editor {
  int needs_exit;
  struct buffer *buff;
};

// Creates a new editor structure with an empty buffer, and starts curses mode
struct editor *editor_new();

// Frees the memory of the editor and ends curses mode
void editor_exit(struct editor *ed);

// Returns 0 if the editor has not been given the order to exit
int editor_needs_exit(struct editor *ed);

// Waits a key input from the user
unsigned int editor_get_key();

// Executes an action according to the key
void editor_interpret_key(struct editor *ed, unsigned int key);

// Refreshes the display of the editor
void editor_refresh(struct editor *ed);

#endif // EDITOR_H
