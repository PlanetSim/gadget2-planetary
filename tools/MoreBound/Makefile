EXEC	= MoreBound

OBJS = main.o globalvars.o readparam.o save.o bound.o particledata.o

# prefix the source directory
SRCOBJS = $(foreach f, $(OBJS), src/$f)

CFLAGS	= -O2 -Wall -Wextra -flto --std=c11  -fsanitize=address -g -I/Users/lx21966/Developer/tracy/public -DTRACY_ENABLE
CXXFLAGS = -O2 -DTRACY_ENABLE --std=c++20 -lstdc++

TRACYOBJ = /Users/lx21966/Developer/tracy/public/TracyClient.o

LIBS	= -lm -lpthread

CC	= clang
CXX = clang++

$(EXEC): $(SRCOBJS) $(TRACYOBJ)
	$(CC) $(CFLAGS) $(TRACYOBJ) $(SRCOBJS) -lstdc++ -o $(EXEC) $(LIBS)

test: $(EXEC)
	time ./$(EXEC) snapshot_720 bndparam.txt
run: $(EXEC)
	time ./$(EXEC) snapshot_010 bndparam.txt
# diff snapshot_720_b.txt test_720.backup.txt

clean :
	rm -f $(SRCOBJS) $(EXEC)
# rm -f $(TRACYOBJ)
