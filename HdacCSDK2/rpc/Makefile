CC	= g++ -std=c++11

HOME	= rpc

INCLUDE	= -I../include -I../include/crypto -I../include/entities \
	-I../include/helpers -I../include/keys -I../include/primitives \
	-I../include/rpc -I../include/script -I../include/structs -I../include/utils

CFLAGS	= 


SRCS	= cif_rpccall.cpp hs_rpc.cpp rpccaller.cpp \
	rpcclient.cpp rpcprotocol.cpp rpcresult.cpp

OBJS  = $(SRCS:.cpp=.o)


lib:	$(OBJS)

.cpp.o:   $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -O2 -c $<


clean:
	rm -f lib$(HOME).a $(OBJS)
