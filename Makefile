SRC=$(wildcard */*.c)
OBJ=$(SRC:%.c=%.o)
%.o: %.c
	$(CC) -c $< -o $@ 
