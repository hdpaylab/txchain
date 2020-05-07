CPP = g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_serialization -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -I.. -I../HdacCSDK2/include -I../lib

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter -fno-strict-aliasing

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


all: speed ex ser1 leveldbex keypair leveldb_test zmq_svr zmq_cli zmq_msvr bigint

ex: ex.cpp
	$(CPP) $(CPPFLAGS) -o $@ ex.cpp $(ALIBS) $(SDKLIBS) $(LIBS)

keypair: keypair.cpp
	$(CPP) $(CPPFLAGS) -o $@ keypair.cpp $(ALIBS) $(SDKLIBS) $(LIBS)

leveldbex: leveldbex.cpp
	$(CPP) $(CPPFLAGS) -o $@ leveldbex.cpp $(LIBS) $(SDKLIBS) 

speed: speed.cpp ../util.cpp 
	$(CPP) $(CPPFLAGS) -o $@ speed.cpp ../util.cpp $(ALIBS) $(SDKLIBS) $(LIBS)

ser1: ser1.cpp ../lib/xsz.cpp
	$(CPP) $(CPPFLAGS) -o $@ ser1.cpp ../lib/xsz.cpp $(LIBS) $(SDKLIBS) 

leveldb_test: ../lib/leveldb.cpp leveldb_test.cpp
	$(CPP) $(CPPFLAGS) -o $@ ../lib/leveldb.cpp leveldb_test.cpp $(LIBS) $(SDKLIBS) 

zmq_msvr: zmq_msvr.cpp
	$(CPP) $(CPPFLAGS) -o $@ zmq_msvr.cpp $(LIBS) $(SDKLIBS) 

zmq_svr: zmq_svr.cpp
	$(CPP) $(CPPFLAGS) -o $@ zmq_svr.cpp $(LIBS) $(SDKLIBS) 

zmq_cli: zmq_cli.cpp
	$(CPP) $(CPPFLAGS) -o $@ zmq_cli.cpp $(LIBS) $(SDKLIBS) 

bigint: bigint_ex.cpp
	$(CPP) $(CPPFLAGS) -o $@ bigint_ex.cpp bigint.c $(LIBS) $(SDKLIBS) 

clean:
	rm -f $(OBJS) *.o *.out *.ver *.log \
	speed ex ser1 leveldbex keypair leveldb_test zmq_svr zmq_cli zmq_msvr bigint
