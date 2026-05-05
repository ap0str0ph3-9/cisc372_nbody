FLAGS= -DDEBUG
LIBS= -lm
ALWAYS_REBUILD=makefile

nbody: nbody.cu compute.cu
	nvcc $(FLAGS) $^ -o $@ $(LIBS) 
clean:
	rm -f *.o nbody 
