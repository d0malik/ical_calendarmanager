CC = gcc
CPPFLAGS=
CFLAGS = -Wall -g -std=c11

all: CalendarParser
	
CalendarParser: CalendarParser.o CalendarHelper.o LinkedListAPI.o
	$(CC) -shared -o ../libparser.so CalendarParser.o CalendarHelper.o LinkedListAPI.o

CalendarParser.o: src/CalendarParser.c
	$(CC) -fPIC $(CFLAGS) -c src/CalendarParser.c src/CalendarHelper.c -Iinclude

CalendarHelper.o: src/CalendarHelper.c
	$(CC) -fPIC $(CFLAGS) -c src/CalendarHelper.c -Iinclude

LinkedListAPI.o: src/LinkedListAPI.c
	$(CC) -fPIC $(CFLAGS) -c src/LinkedListAPI.c -Iinclude

clean:
	rm -rf CalendarParser *.o *.so ../*.so
