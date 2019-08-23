CC=g++ -std=c++11
DEPS = bound.h
OBJ = bound.o main.o 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< 

bound: $(OBJ)
	$(CC) -o $@ $^ 

clean:
	rm *.o
