CC=g++ -std=c++11
DEPS = dataset.h
OBJ = dataset.o main.o 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< 

dataset: $(OBJ)
	$(CC) -o $@ $^ 

clean:
	rm *.csv *.txt *.o
