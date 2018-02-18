INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt

HFILES= 
CFILES= main.c all_threads.c my_timer.c doubly_ll.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	pthread_exe 

clean:
	-rm -f *.o *.d
	-rm -f pthread_exe mylog.txt

pthread_exe: $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJS) -o $@ $(LIBS)


depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
