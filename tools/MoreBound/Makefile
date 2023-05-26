EXEC	= MoreBound

OBJS	= main.o globalvars.o \
	  readparam.o load.o \
	  save.o bound.o\
	  nrsrc/nrutil.o

INCL	= globalvars.h nrsrc/nrutil.h

CFLAGS	= -O3 

LIBS	= -lm

CC	= gcc

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LIBS)

$(OBJS): $(INCL)

test: $(EXEC)
	time ./$(EXEC) snapshot_720 bndparam.txt
	diff snapshot_720_b.txt test_720.backup.txt

clean :
	rm -f *.o $(EXEC)
