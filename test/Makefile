CPP = g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_serialization -lboost_thread -lpthread
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


all: speed ex ser1 ser2 leveldbex 

ex: ex.cpp
	$(CPP) $(CPPFLAGS) -o $@ ex.cpp $(ALIBS) $(SDKLIBS) $(LIBS)

leveldbex: leveldbex.cpp
	$(CPP) $(CPPFLAGS) -o $@ leveldbex.cpp $(LIBS) $(SDKLIBS) 

speed: speed.cpp ../util.cpp 
	$(CPP) $(CPPFLAGS) -o $@ speed.cpp ../util.cpp $(ALIBS) $(SDKLIBS) $(LIBS)

ser1: ser1.cpp
	$(CPP) $(CPPFLAGS) -o $@ ser1.cpp $(LIBS) $(SDKLIBS) 

ser2: ser2.cpp
	$(CPP) $(CPPFLAGS) -o $@ ser2.cpp $(LIBS) $(SDKLIBS) 

clean:
	rm -f ex leveldbex speed serialize_ex $(OBJS) *.o *.out *.ver *.log
