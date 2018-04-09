EXEC   = Escaping

SRCS   = escaping.c loadsnap.c globvars.c \
	 indextable.c nrutils/nrutil.c

OBJS   = $(SRCS:.c=.o)
INCL   = globvars.h nrutils/nrutil.h

.KEEP_STATE:

OPT    =  # -DTJ_VERSION

CFLAGS = $(OPT) -g     # -O2 

LIBS   =  -lm

CC     =  cc      # gcc , or cc on NERSC


$(EXEC): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $(EPATH)$(EXEC)  $(LIBS)
	rm -f $(OBJS)

$(OBJS): $(INCL) 


clean:
	rm -f $(OBJS) $(EXEC)

