CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

SRCS=proj2.c
TARGET=proj2

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
clean:
	rm -f $(TARGET)
	rm -f $(TARGET).zip
	rm -f $(TARGET).out
pack:
	rm -f $(TARGET).zip
	zip -r $(TARGET).zip *.c Makefile
.PHONY: clean pack all