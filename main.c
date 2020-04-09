#include "editor.h"
#include "util.h"

int main() {
  if (STDIN_IS_TERMINAL()) { // To prevent input of pipes and files
    struct editor *ed = editor_new();
  
    while(!editor_needs_exit(ed)) {
      editor_interpret_key(ed, editor_get_key());
      editor_refresh(ed);
    }
    
    editor_exit(ed);
    return 0;
  } else {
    fprintf(stderr, "ERROR: The input is not a terminal\n");
    return -1;
  }
}
