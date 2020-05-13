CPP	= g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -I../HdacCSDK2/include -I../lib

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter

SRCS2	= main.cpp pub.cpp sub.cpp verify.cpp ldbio.cpp \
	../lib/params.cpp ../lib/leveldb.cpp ../lib/xserial.cpp ../lib/util.cpp

OBJS2	= $(SRCS2:.cpp=.o)

HDACLIB	= ../HdacCSDK2/hdaclib.a


all: tx2 cli

tx2: $(OBJS2)
	$(CC) $(CPPFLAGS) -o $@ $(OBJS2) $(HDACLIB) $(SDKLIBS) $(LIBS) 

cli: cli.o ../lib/params.cpp
	$(CC) $(CPPFLAGS) -o $@ cli.o ../lib/params.cpp $(HDACLIB) $(SDKLIBS) $(LIBS) 

clean:
	rm -f tx2 cli $(OBJS) *.o *.out *.ver *.a *.log
	rm -rf test.db
