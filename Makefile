EXECUTABLE = quadtree

CC = gcc

CFLAGS = -Wall -Wextra -std=c11

SOURCES = quadtree.c

build:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

run:
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)
