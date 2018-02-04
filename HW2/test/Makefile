INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= 

HFILES= 
CFILES= test_ll.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	test_ll

clean:
	-rm -f *.o *.d
	-rm -f test_ll

test_ll: test_ll.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o 

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
