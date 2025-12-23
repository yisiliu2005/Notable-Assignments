CC = gcc


CFLAGS = -Wall -g
TARGET = fs_simulator


SRCS = main.c fs_indexed.c
OBJS = $(SRCS:.c=.o)
HEADERS = fs_indexed.h


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

fs_indexed.o: fs_indexed.c $(HEADERS)
	$(CC) $(CFLAGS) -c fs_indexed.c

clean:
	rm -f $(OBJS) $(TARGET) 