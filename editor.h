#ifndef EDITOR_H
#define EDITOR_H

struct Editor {
  int needs_exit;
  struct Buffer *buff;
};

struct Editor *editor_new();
void editor_exit();
int editor_needs_exit(struct Editor *ed);
int editor_get_key();
void editor_interpret_key(struct Editor *ed, int key);
void editor_refresh(struct Editor *ed);

#endif // EDITOR_H
