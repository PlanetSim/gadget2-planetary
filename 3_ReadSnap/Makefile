EXEC   = Read 

SRCS   = read.c loadsnap.c globvars.c pressure.c eos_table_interp.c \

OBJS   = $(SRCS:.c=.o)
INCL   = globvars.h eos.h

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

