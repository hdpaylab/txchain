CPP	= g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -I../HdacCSDK2/include -I../lib -I../lib2

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter

SRCS2	= main.cpp pub.cpp sub.cpp verify.cpp ldbio.cpp seriz.cpp \
	../lib/params.cpp ../lib/leveldb.cpp ../lib/xsz.cpp ../lib/util.cpp

OBJS2	= $(SRCS2:.cpp=.o)

ALIBS	= \
	$(CSDKDIR)/lib/libhelpers.a \
	$(CSDKDIR)/lib/libkeys.a \
	$(CSDKDIR)/lib/libprimitives.a \
	$(CSDKDIR)/lib/librpc.a \
	$(CSDKDIR)/lib/libscripts.a \
	$(CSDKDIR)/lib/libentities.a \
	$(CSDKDIR)/lib/libstructs.a \
	$(CSDKDIR)/lib/libutils.a \
	$(CSDKDIR)/lib/libcrypto.a \
	../lib2/hashlib.a


all: tx2 cli

tx2: $(OBJS2)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS2) $(ALIBS) $(SDKLIBS) $(LIBS) 

cli: cli.o seriz.cpp ../lib/params.cpp ../lib/xsz.cpp ../lib/util.cpp
	$(CPP) $(CPPFLAGS) -o $@ cli.o seriz.cpp ../lib/params.cpp ../lib/xsz.cpp ../lib/util.cpp \
	$(ALIBS) $(SDKLIBS) $(LIBS) 

clean:
	rm -f tx2 cli $(OBJS) *.o *.out *.ver *.a *.log
	rm -rf test.db
