CC = gcc
CFLAGS =-Wall -lpthread -pedantic
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)
objects = server.o


chat : $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) -o server $(objects)

main : chat.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c server.c

.PHONY : clean
clean :
	-rm server $(objects)
