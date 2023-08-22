EXE := Weaver
WARNS := -Wall -Wextra -Wpedantic
FLAGS := -Og -g

.PHONY:

$(EXE): driver.o read.o reader.o alloc.o print.o env.o eval.o
	cc -o $@ $^

.PHONY: .c.o

.c.o:
	cc -c -o $@ $< $(WARNS) $(FLAGS)

driver.o: driver.c
read.o: read.c
reader.o: reader.c
alloc.o: alloc.c
print.o: print.c
env.o: env.c
eval.o: eval.c

clean:
	rm -rf *.o $(EXE) || true
