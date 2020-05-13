CPP	= g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_serialization -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb -lstdc++

INCLUDE	= -I/usr/include/postgresql -I../HdacCSDK2/include -I../lib -I../lib2

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter -fno-strict-aliasing

SRCS2	= main.cpp pub.cpp sub.cpp verify.cpp ldbio.cpp seriz.cpp 

OBJS2	= $(SRCS2:.cpp=.o)


TXLIB	= ../lib/txlib.a ../HdacCSDK2/hdaclib.a


all: tx2 cli

tx2: $(OBJS2)
	$(CC) $(CPPFLAGS) -o $@ $(OBJS2) $(TXLIB) $(SDKLIBS) $(LIBS) 

cli: cli.o seriz.cpp 
	$(CC) $(CPPFLAGS) -o $@ cli.o seriz.cpp $(TXLIB) $(SDKLIBS) $(LIBS) 

clean:
	rm -f tx2 cli $(OBJS) *.o *.out *.ver *.a *.log
	rm -rf test.db
