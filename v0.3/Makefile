CPP	= g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -I../HdacCSDK2/include -I../lib -I../lib2

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter

SRCS2	= main.cpp pub.cpp sub.cpp verify.cpp ldbio.cpp seriz.cpp \
	../lib/params.cpp ../lib/leveldb.cpp ../lib/xserial.cpp ../lib/util.cpp

OBJS2	= $(SRCS2:.cpp=.o)


HDACLIB	= ../HdacCSDK2/hdaclib.a

TXLIB	= ../lib/txlib.a


all: tx2 cli

tx2: $(OBJS2)
	$(CC) $(CPPFLAGS) -o $@ $(OBJS2) $(HDACLIB) $(TXLIB) $(SDKLIBS) $(LIBS) 

cli: cli.o seriz.cpp ../lib/params.cpp ../lib/xserial.cpp ../lib/util.cpp
	$(CC) $(CPPFLAGS) -o $@ cli.o seriz.cpp ../lib/params.cpp ../lib/xserial.cpp ../lib/util.cpp \
	$(HDACLIB) $(TXLIB) $(SDKLIBS) $(LIBS) 

clean:
	rm -f tx2 cli $(OBJS) *.o *.out *.ver *.a *.log
	rm -rf test.db
