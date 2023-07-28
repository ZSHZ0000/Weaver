EXE := Weaver
FLAGS := -Og -g

.PHONY:

$(EXE): reader.o
	cc -o $@ $^

.PHONY: .c.o

.c.o:
	cc -c -o $@ $< $(FLAGS) 

reader.o: reader.c

clean:
	rm -rf *.o $(EXE) || true
