EXEC    = InitPlanet

OBJS    = main.o calcentropy.o findxyz.o initializeplanet.o \
          interpolate.o makeparticles.o readtable.o writeics.o \
          globalvars.o readparam.o tprofile.o writeprofiles.o

INCL    = prototypes.h globalvars.h

CFLAGS  = -O3

LIBS    = -lm

CC      = gcc


$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)  $(LIBS)

$(OBJS): $(INCL)

clean :
	rm -f *.o $(EXEC)

