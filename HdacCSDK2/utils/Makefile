CC	= g++ -std=c++11 

HOME	= utils

INCLUDE	= -I../include -I../include/crypto -I../include/entities \
	-I../include/helpers -I../include/keys -I../include/primitives \
	-I../include/rpc -I../include/script -I../include/structs -I../include/utils

CFLAGS	= 


SRCS	= allocators.cpp base64.cpp profiler.cpp random.cpp tools.cpp \
	util.cpp utility.cpp utilsfront.cpp utilstrencodings.cpp utiltime.cpp

OBJS  = $(SRCS:.cpp=.o)


lib:	$(OBJS)

.cpp.o:   $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -O2 -c $<


clean:
	rm -f lib$(HOME).a $(OBJS)
