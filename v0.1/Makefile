CC	= g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb -lstdc++

INCLUDE	= -I/usr/include/postgresql -I../HdacCSDK2/include -I../lib

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter

SRCS	= xmain.cpp xpub.cpp xsub.cpp xverify.cpp xldbio.cpp 

OBJS	= $(SRCS:.cpp=.o)


TXLIB	= ../lib/txlib.a ../HdacCSDK2/hdaclib.a


all: lib tx 

lib: $(TXLIB)

$(TXLIB): 
	cd $(CSDKDIR); make; cd ..

tx: $(OBJS)
	$(CC) $(CPPFLAGS) -o $@ $(OBJS) $(TXLIB) $(SDKLIBS) $(LIBS)

clean:
	rm -f tx $(OBJS) *.o *.out *.ver *.a *.log
	rm -rf test.db 
