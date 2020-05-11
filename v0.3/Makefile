CPP	= g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -I../HdacCSDK2/include -I../lib

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter

SRCS2	= main.cpp pub.cpp sub.cpp verify.cpp ldbio.cpp \
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


all: tx2 cli

tx2: $(OBJS2)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS2) $(ALIBS) $(SDKLIBS) $(LIBS) 

cli: cli.o ../lib/params.cpp ../lib/xsz.cpp ../lib/util.cpp
	$(CPP) $(CPPFLAGS) -o $@ cli.o ../lib/params.cpp ../lib/xsz.cpp ../lib/util.cpp \
	$(ALIBS) $(SDKLIBS) $(LIBS) 

bak:
	tar cf ~/BAK/txchain-v0.3-`date +"%y%m%d"`.tar M* *.[hcy]* *.sh README 
	gzip -f ~/BAK/txchain-v0.3-`date +"%y%m%d"`.tar

clean:
	rm -f tx2 cli $(OBJS) *.o *.out *.ver *.a *.log
	rm -rf test.db
