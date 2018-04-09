EXEC   = Snap2ss 

SRCS   = read.c loadsnap.c globvars.c ssio.c pressure.c eos_table_interp.c nrsrc/nrutil.c \

OBJS   = $(SRCS:.c=.o)
INCL   = globvars.h eos.h nrsrc/nrutil.h ssdefs.h ssio.h

.KEEP_STATE:

OPT    =  # -DTJ_VERSION

CFLAGS = $(OPT) -g     # -O2 

LIBS   =  -lm

CC     =  gcc      # gcc , or cc on NERSC


$(EXEC): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $(EPATH)$(EXEC)  $(LIBS)
	rm -f $(OBJS)

$(OBJS): $(INCL) 


clean:
	rm -f $(OBJS) $(EXEC)

