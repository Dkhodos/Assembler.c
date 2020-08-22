CC = gcc
CFLAGS = -Wall -pedantic -ansi
DEPS = asmObject.h util.h errorHandler.h symbolTable.h memoryStack.h assembler.h
OBJ = main.o assembler.o asmObject.o util.o errorHandler.o symbolTable.o memoryStack.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

assembler: $(OBJ)
	gcc $(CFLAGS) -o $@ $^

clean:
	rm *.o
	rm assembler