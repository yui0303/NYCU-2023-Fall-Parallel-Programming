CC=g++
SRC=pi.cpp
FLAGS=-O3 -std=c++17 -Wall -pthread
TARGET=pi.out

all: pi

pi: pi.cpp
	$(CC) $(FLAGS) $(SRC) -o $(TARGET)

.PHONY: clean
clean:
	rm -f *.o *~ $(TARGET) gmon.out 
