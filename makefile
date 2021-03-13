exec = main.out
sources = $(wildcard src/*.c)
objects = $(sources: .c=.o)
$(exec): $(objects)
	mpicc $(objects) -o $(exec)

%.o: %.c include/%.h
	mpicc -c $< -o $@

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o
