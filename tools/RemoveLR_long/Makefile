EXEC	= RemoveLR_long

OBJS	= main.o globalvars.o \
	  readparam.o load.o \
	  write.o bound.o\
	  nrsrc/nrutil.o

INCL	= globalvars.h nrsrc/nrutil.h

CFLAGS	= -O3 

LIBS	= -lm

CC	= gcc

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LIBS)

$(OBJS): $(INCL)

clean :
	rm -f *.o $(EXEC)
