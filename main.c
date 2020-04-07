#include "editor.h"

int main() {
  struct editor *ed = editor_new();
  
  while(!editor_needs_exit(ed)) {
    editor_interpret_key(ed, editor_get_key());
    editor_refresh(ed);
  }

  editor_exit(ed);
  
  return 0;
}
