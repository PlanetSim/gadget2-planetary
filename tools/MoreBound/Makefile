EXEC	= MoreBound

OBJS = main.o globalvars.o readparam.o save.o bound.o particledata.o

# prefix the source directory
SRCOBJS = $(foreach f, $(OBJS), src/$f)

CFLAGS	= -O2 -Wall -Wextra -flto

LIBS	= -lm

CC	= clang

$(EXEC): $(SRCOBJS)
	$(CC) $(CFLAGS) $(SRCOBJS) -o $(EXEC) $(LIBS)

test: $(EXEC)
	time ./$(EXEC) snapshot_720 bndparam.txt
# diff snapshot_720_b.txt test_720.backup.txt

clean :
	rm -f $(SRCOBJS) $(EXEC)
