CC=g++ -std=c++11 -fopenmp -mcmodel=medium
THREAD = -fsanitize=thread -g
DEPS = Bidirectional.h
OBJ = Bidirectional.o main.o 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(THREAD) 

bidirectional: $(OBJ)
	$(CC) -o $@ $^ $(THREAD)

clean:
	rm *.o
