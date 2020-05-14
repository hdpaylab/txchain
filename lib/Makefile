CC	= g++ -O

INCLUDE	= -I../lib -I../HdacCSDK2/include

CFLAGS	= -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter -fno-strict-aliasing


SRCS	= lib.c aes.c base58.c base64.cpp md5.c rc4.c sha1.c sha256.c sha512.c bigint.c \
	leveldb.cpp params.cpp util.cpp xserial.cpp

COBJS	= lib.o aes.o base58.o base64.o md5.o rc4.o sha1.o sha256.o sha512.o bigint.o 

CPPOBJS	= leveldb.o params.o util.o xserial.o


all:	txlib.a 

txlib.a: $(COBJS) $(CPPOBJS)
	rm -f $@
	ar cru $@ $(COBJS) $(CPPOBJS)

.cpp.o: $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -O2 -c $<

.c.o: $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -O2 -c $<

clean:
	rm -f *.o txlib.a
