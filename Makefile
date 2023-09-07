EXE := Weaver
WARNS := -Wall -Wextra -Wpedantic
FLAGS := -Og -g
CC := cc

.PHONY:
$(EXE): src/driver.o src/read.o src/reader.o src/alloc.o src/print.o src/env.o src/eval.o src/built-in.o
	$(CC) -o $@ $^

.PHONY: .c.o clean

.c.o:
	$(CC) -c -o $@ $< $(WARNS) $(FLAGS)

src/driver.o: src/driver.c
src/read.o: src/read.c
src/reader.o: src/reader.c
src/alloc.o: src/alloc.c
src/print.o: src/print.c
src/env.o: src/env.c
src/eval.o: src/eval.c
src/built-in.src/o: built-in.c

clean:
	rm -rf src/*.o $(EXE) || true
