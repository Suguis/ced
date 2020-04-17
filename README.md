# ced
A simple curses text editor written in C.

# Controls
- Arrow keys: move cursor.
- Home/End: go to the beggining/end of line.
- Supr: remove the character under the cursor.
- Return: insert a new line.
- Backspace: remove the character before the cursor.
- Ctrl+S: save the file.
- Esc: exit the program (make sure you save first).

# How it works
Simply compile it with make and write
```
./ced <file>
```
to edit the desired file. You can also save with an in-memory buffer you can't save executing
```
./ced
```
