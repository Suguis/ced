#ifndef EDITOR_H
#define EDITOR_H

#define KEY_ESC 27
#define ESC_DELAY 10

#define TEXT_PAIR 1
#define STATUS_PAIR 2

struct editor {
  int needs_exit;
  int colors_enabled;
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

// Draws the text of the buffer on the display
void editor_draw_text(struct editor *ed);


#endif // EDITOR_H
