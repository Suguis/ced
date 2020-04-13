#include "editor.h"
#include "util.h"

int main(int argc, char **argv) {
  if (STDIN_IS_TERMINAL()) { // To prevent input of pipes and files

    char *filename = argc > 1 ? argv[1] : NULL;
    struct editor *ed = editor_new(filename);
    
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
