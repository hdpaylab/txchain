CC	= g++ -std=c++11

HOME	= keys

INCLUDE	= -I../include -I../include/crypto -I../include/entities \
	-I../include/helpers -I../include/keys -I../include/primitives \
	-I../include/rpc -I../include/script -I../include/structs -I../include/utils

CFLAGS	= 


SRCS	= bitcoinaddress.cpp bitcoinsecret.cpp eccautoinitreleasehandler.cpp \
	hs_keys.cpp hs_keys_wrapper.cpp key.cpp keyslib.cpp multisig.cpp \
	pubkey.cpp rawmetadata.cpp standard.cpp transactions.cpp

OBJS  = $(SRCS:.cpp=.o)


lib:	$(OBJS)

.cpp.o:   $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -O2 -c $<


clean:
	rm -f lib$(HOME).a $(OBJS)
