CC	= g++ -std=c++11

HOME	= primitives

INCLUDE	= -I../include -I../include/crypto -I../include/entities \
	-I../include/helpers -I../include/keys -I../include/primitives \
	-I../include/rpc -I../include/script -I../include/structs -I../include/utils

CFLAGS	= 


SRCS	= analyzetx.cpp interpreter.cpp transaction.cpp

OBJS  = $(SRCS:.cpp=.o)


lib:	$(OBJS)

.cpp.o:   $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -O2 -c $<


clean:
	rm -f lib$(HOME).a $(OBJS)
