PROG = cdcap
OBJS = cdcap.o log.o
SRCS = cdcap.c log.c

CC = gcc
CFLAGS = -Wall -O2 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
LDFLAGS = 
LIBS =

$(PROG): $(OBJS) $(SRCS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROG) $(LIBS)
	strip $(PROG)

clean:
	rm -f $(PROG) *.o *~
