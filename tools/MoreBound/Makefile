EXE	= MoreBound
OBJS = main.o globalvars.o readparam.o save.o bound.o particledata.o

# prefix the source directory
SRCOBJS = $(foreach f, $(OBJS), src/$f)
CFLAGS	= -O2 -Wall -Wextra -flto --std=c11
CXXFLAGS = -O2 -DTRACY_ENABLE --std=c++20 -lstdc++

LIBS	= -lm -lpthread

CC	= clang
CXX = clang++

TRACYOBJ = /Users/lx21966/Developer/tracy/public/TracyClient.o

$(EXEC): $(SRCOBJS)
	$(CC) $(CFLAGS) $(SRCOBJS) -o $(EXEC) $(LIBS)

debug: CFLAGS += -fsanitize=address -g -I/Users/lx21966/Developer/tracy/public -DTRACY_ENABLE 
debug: $(SRCOBJS) $(TRACYOBJ)
	$(CC) $(CFLAGS) $(TRACYOBJ) $(SRCOBJS) -lstdc++ -o $(EXEC) $(LIBS)

format:
	clang-format -i src/*.c src/*.h

test: debug
	time ./$(EXEC) snapshot_720 bndparam.txt

run: debug
	time ./$(EXEC) snapshot_010 bndparam.txt

clean :
	rm -f $(SRCOBJS) $(EXEC)
# rm -f $(TRACYOBJ)
