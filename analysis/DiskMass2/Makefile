EXEC	= DiskMass

OBJS	= main.o globalvars.o \
	  readparam.o load.o \
	  centermass.o bound.o \
	  save.o diskmass.o \
	  nrsrc/nrutil.o

INCL	= globalvars.h nrsrc/nrutil.h

CLFAGS	= -O3

LIBS	= -lm

CC	= gcc

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LIBS)

$(OBJS): $(INCL)

clean :
	rm -f *.o $(EXEC)
