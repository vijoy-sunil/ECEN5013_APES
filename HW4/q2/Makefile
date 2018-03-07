INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -pthread -lrt

HFILES= 
CFILES= ipc_demo.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	ipc_demo 

clean:
	-rm -f *.o *.d
	-rm -f ipc_demo

ipc_demo: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LIBS) $(CFLAGS)


depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
