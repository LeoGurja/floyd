exec = main.out
sources = $(wildcard src/*.c)
objects = $(sources: .c=.o)
$(exec): $(objects)
	gcc $(objects) -o $(exec) -fopenmp

%.o: %.c include/%.h
	gcc -c $< -o $@

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o

single:
	gcc single_thread.c src/matrix.c src/utils.c -o single.out
