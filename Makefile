CC = gcc
CFLAGS = -Wall -lpthread
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)
objects = client.o


chat : $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) -o client $(objects)

main : chat.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c client.c

.PHONY : clean
clean :
	-rm client $(objects)