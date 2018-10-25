CC = gcc
CFLAGS = -Wall -lpthread
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)
objects = cliente.o


chat : $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) -o cliente $(objects)

main : chat.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c cliente.c

.PHONY : clean
clean :
	-rm cliente $(objects)