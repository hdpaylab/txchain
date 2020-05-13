CPP = g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_serialization -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I./ -I/usr/include/postgresql -I../HdacCSDK2/include -I../lib

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

TXLIB	= ../lib/txlib.a


all: bigint_ex keypair keypair_ex leveldb_test leveldbex \
	rapidjson_ex xsz_ex speed xsz_ex \
	zmq_cli zmq_msvr zmq_svr \
	test_aes test_base58 test_base64 test_hashlib test_md5 \
	test_rc4 test_sha1 test_sha256 test_sha512

bigint_ex: bigint_ex.cpp $(TXLIB)
	$(CPP) $(CPPFLAGS) -o $@ bigint_ex.cpp $(TXLIB) $(LIBS) $(SDKLIBS) 

keypair: keypair.cpp $(TXLIB)
	$(CPP) $(CPPFLAGS) -o $@ keypair.cpp $(ALIBS) $(TXLIB) $(SDKLIBS) $(LIBS)

keypair_ex: keypair_ex.cpp $(TXLIB)
	$(CPP) $(CPPFLAGS) -o $@ keypair_ex.cpp $(ALIBS) $(TXLIB) $(SDKLIBS) $(LIBS)

leveldb_test: leveldb_test.cpp $(TXLIB)
	$(CPP) $(CPPFLAGS) -o $@ leveldb_test.cpp $(TXLIB) $(LIBS) $(SDKLIBS) 

leveldbex: leveldbex.cpp
	$(CPP) $(CPPFLAGS) -o $@ leveldbex.cpp $(LIBS) $(SDKLIBS) 

rapidjson_ex: rapidjson_ex.o
	$(CPP) $(CPPFLAGS) -o $@ rapidjson_ex.cpp $(LIBS) $(SDKLIBS) 

speed: speed.cpp $(TXLIB)
	$(CPP) $(CPPFLAGS) -o $@ speed.cpp $(ALIBS) $(TXLIB) $(SDKLIBS) $(LIBS)

xsz_ex: xsz_ex.cpp $(TXLIB)
	$(CPP) $(CPPFLAGS) -o $@ xsz_ex.cpp $(LIBS) $(TXLIB) $(SDKLIBS) 

zmq_cli: zmq_cli.cpp
	$(CPP) $(CPPFLAGS) -o $@ zmq_cli.cpp $(LIBS) $(SDKLIBS) 

zmq_msvr: zmq_msvr.cpp
	$(CPP) $(CPPFLAGS) -o $@ zmq_msvr.cpp $(LIBS) $(SDKLIBS) 

zmq_svr: zmq_svr.cpp
	$(CPP) $(CPPFLAGS) -o $@ zmq_svr.cpp $(LIBS) $(SDKLIBS) 


test_aes: test_aes.c $(TXLIB)
	$(CPP) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_base58: test_base58.c $(TXLIB)
	$(CPP) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_base64: test_base64.c $(TXLIB)
	$(CPP) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_hashlib: test_hashlib.c $(TXLIB)
	$(CPP) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_md5: test_md5.c $(TXLIB)
	$(CPP) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_rc4: test_rc4.c $(TXLIB)
	$(CPP) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_sha1: test_sha1.c $(TXLIB)
	$(CPP) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_sha256: test_sha256.c $(TXLIB)
	$(CPP) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_sha512: test_sha512.c $(TXLIB)
	$(CPP) $(CPPFLAGS) $@.c -o $@ $(TXLIB)


clean:
	rm -f $(OBJS) *.o *.out *.ver *.log
	rm -f bigint_ex keypair keypair_ex leveldb_test leveldbex \
		rapidjson_ex xsz_ex speed xsz_ex \
		zmq_cli zmq_msvr zmq_svr \
		test_aes test_base58 test_base64 test_hashlib test_md5 \
		test_rc4 test_sha1 test_sha256 test_sha512
	rm -rf level.db
