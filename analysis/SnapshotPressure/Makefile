EXEC	= Snapshot

OBJS	= main.o globalvars.o \
	  readparam.o load.o \
	  pressure.o save.o \
	  eos_table_interp.o \
	  nrsrc/nrutil.o

INCL	= globalvars.h nrsrc/nrutil.h eos.h

CFLAGS	= -O3

LIBS	= -lm

CC	= gcc

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LIBS)

$(OBJS): $(INCL)

clean :
	rm -f *.o $(EXEC)
