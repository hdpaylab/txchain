CC	= g++ -std=c++11
AR	= ar 

INCLUDE	= -I../include -I../include/crypto -I../include/entities \
	-I../include/helpers -I../include/keys -I../include/primitives \
	-I../include/rpc -I../include/script -I../include/structs -I../include/utils

CFLAGS	= 


SRCS	= helpers.cpp keys.cpp primitives.cpp rpc.cpp \
	test_module.cpp perfTxGen.cpp \
	test_verify.cpp \
	test.cpp

OBJS	= $(SRCS:.cpp=.o)

ALIBS	= \
	../lib/libhelpers.a \
	../lib/libkeys.a \
	../lib/libprimitives.a \
	../lib/librpc.a \
	../lib/libscripts.a \
	../lib/libentities.a \
	../lib/libstructs.a \
	../lib/libutils.a \
	../lib/libcrypto.a \


LIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread


TARGET	= helpers keys primitives rpc test_module test_verify test


all:	$(TARGET)

helpers: helpers.o
	$(CC) $(CFLAGS) -O2 -o $@ $< $(ALIBS) $(LIBS)
	exit

keys:	keys.o
	$(CC) $(CFLAGS) -O2 -o $@ $< $(ALIBS) $(LIBS) 

primitives: primitives.o
	$(CC) $(CFLAGS) -O2 -o $@ $< $(ALIBS) $(LIBS)

rpc:	rpc.o
	$(CC) $(CFLAGS) -O2 -o $@ $< $(ALIBS) $(LIBS)

test_module: test_module.o
	$(CC) $(CFLAGS) -O2 -o $@ $< $(ALIBS) $(LIBS)

test_verify: test_verify.o
	$(CC) $(CFLAGS) -O2 -o $@ $< $(ALIBS) $(LIBS)

test:	test.o
	$(CC) $(CFLAGS) -O2 -o $@ $< $(ALIBS) $(LIBS) 


.cpp.o:   $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -O2 -c $<


clean:
	rm -f $(OBJS) helpers keys primitives rpc test test_module test_verify
