CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

SRCS=proj2.c
TARGET=proj2

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

run:
	./$(TARGET) 1000 5 50 696 210
clean:
	rm -f $(TARGET)
pack:
	tar -cvf $(TARGET).zip *.c Makefile
.PHONY: clean run all