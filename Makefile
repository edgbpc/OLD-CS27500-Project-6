GCC = gcc
CFLAGS = -g 
TARGET = mysh
OBJS = mysh.o  makeargv.o
.SUFFIXES: .c .o

mysh: $(OBJS) $(HDRS)
	        $(GCC) $(CFLAGS) $(OBJS) -o $@

.c.o:
	        $(GCC) $(CFLAGS) -c $<


clean:
	       rm -f *.o *~ $(TARGET) $(OBJS)


