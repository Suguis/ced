EXEC=ced
OBJ=main.o editor.o buffer.o util.o
LIB=-lncursesw
FLAGS=-g -Wall

ced: $(OBJ)
	$(CC) $^ -o $(EXEC) $(FLAGS) $(LIB)

%.o: %.c
	$(CC) -c $(FLAGS) $<

clean:
	rm -rf $(OBJ) $(EXEC)
