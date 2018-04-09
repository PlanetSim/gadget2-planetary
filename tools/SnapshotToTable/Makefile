EXEC   = SnapToTable

SRCS   = snaptotable.c loadsnap.c globvars.c eos_table_interp.c\

OBJS   = $(SRCS:.c=.o)
INCL   = globvars.h

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

