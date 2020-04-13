CPP = g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -I.. -I../HdacCSDK2/include

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings 

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


all: ex speed

ex: ex.cpp
	$(CPP) $(CPPFLAGS) -o $@ ex.cpp $(ALIBS) $(SDKLIBS) $(LIBS)

speed: speed.cpp ../util.cpp
	$(CPP) $(CPPFLAGS) -o $@ speed.cpp ../util.cpp $(ALIBS) $(SDKLIBS) $(LIBS)

clean:
	rm -f ex speed $(OBJS) *.o *.out *.ver *.log
