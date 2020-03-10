CC	= g++ -std=c++11
AR	= ar 

HOME	= crypto

INCLUDE	= -I../include -I../include/crypto -I../include/entities \
	-I../include/helpers -I../include/keys -I../include/primitives \
	-I../include/rpc -I../include/script -I../include/structs -I../include/utils

CFLAGS	= 


SRCS	= hmac_sha256.cpp hmac_sha512.cpp ripemd160.cpp \
	sha1.cpp sha256.cpp sha512.cpp

OBJS  = $(SRCS:.cpp=.o)


lib:	lib$(HOME).a

lib$(HOME).a:	$(OBJS)
	rm -f $@
	$(AR) cru $@ $(OBJS)
	cd ../lib; rm -f lib$(HOME).a; ln -f -s ../$(HOME)/*.a .

.cpp.o:   $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -O2 -c $<


clean:
	rm -f lib$(HOME).a $(OBJS)
