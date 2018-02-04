INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= 

HFILES= 
CFILES= doubly_ll.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	doubly_ll

clean:
	-rm -f *.o *.d
	-rm -f doubly_ll

doubly_ll: doubly_ll.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o 

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
