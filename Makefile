EXE := Weaver
FLAGS := -Og -g

.PHONY:

$(EXE): driver.o read.o reader.o alloc.o print.o
	cc -o $@ $^

.PHONY: .c.o

.c.o:
	cc -c -o $@ $< $(FLAGS) 

driver.o: driver.c
read.o: read.c
reader.o: reader.c
alloc.o: alloc.c
print.o: print.c

clean:
	rm -rf *.o $(EXE) || true
