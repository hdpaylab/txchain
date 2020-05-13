CC = g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_serialization -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I./ -I/usr/include/postgresql -I../HdacCSDK2/include -I../lib

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter -fno-strict-aliasing


TXLIB	= ../lib/txlib.a ../HdacCSDK2/hdaclib.a


all: bigint_ex keypair keypair_ex leveldb_test leveldbex \
	rapidjson_ex xsz_ex speed xsz_ex \
	zmq_cli zmq_msvr zmq_svr test_params \
	test_aes test_base58 test_base64 test_hashlib test_md5 \
	test_rc4 test_sha1 test_sha256 test_sha512

bigint_ex: bigint_ex.cpp $(TXLIB)
	$(CC) $(CPPFLAGS) -o $@ bigint_ex.cpp $(TXLIB) $(LIBS) $(SDKLIBS) 

keypair: keypair.cpp $(TXLIB)
	$(CC) $(CPPFLAGS) -o $@ keypair.cpp $(TXLIB) $(SDKLIBS) $(LIBS)

keypair_ex: keypair_ex.cpp $(TXLIB)
	$(CC) $(CPPFLAGS) -o $@ keypair_ex.cpp $(TXLIB) $(SDKLIBS) $(LIBS)

leveldb_test: leveldb_test.cpp $(TXLIB)
	$(CC) $(CPPFLAGS) -o $@ leveldb_test.cpp $(TXLIB) $(LIBS) $(SDKLIBS) 

leveldbex: leveldbex.cpp
	$(CC) $(CPPFLAGS) -o $@ leveldbex.cpp $(LIBS) $(SDKLIBS) 

rapidjson_ex: rapidjson_ex.o
	$(CC) $(CPPFLAGS) -o $@ rapidjson_ex.cpp $(LIBS) $(SDKLIBS) 

speed: speed.cpp $(TXLIB)
	$(CC) $(CPPFLAGS) -o $@ speed.cpp $(TXLIB) $(SDKLIBS) $(LIBS)

xsz_ex: xsz_ex.cpp $(TXLIB)
	$(CC) $(CPPFLAGS) -o $@ xsz_ex.cpp $(LIBS) $(TXLIB) $(SDKLIBS) 

zmq_cli: zmq_cli.cpp
	$(CC) $(CPPFLAGS) -o $@ zmq_cli.cpp $(LIBS) $(SDKLIBS) 

zmq_msvr: zmq_msvr.cpp
	$(CC) $(CPPFLAGS) -o $@ zmq_msvr.cpp $(LIBS) $(SDKLIBS) 

zmq_svr: zmq_svr.cpp
	$(CC) $(CPPFLAGS) -o $@ zmq_svr.cpp $(LIBS) $(SDKLIBS) 

test_params: test_params.cpp
	$(CC) $(CPPFLAGS) -o $@ test_params.cpp $(TXLIB) $(LIBS) $(SDKLIBS) 


test_aes: test_aes.c $(TXLIB)
	$(CC) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_base58: test_base58.c $(TXLIB)
	$(CC) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_base64: test_base64.c $(TXLIB)
	$(CC) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_hashlib: test_hashlib.c $(TXLIB)
	$(CC) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_md5: test_md5.c $(TXLIB)
	$(CC) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_rc4: test_rc4.c $(TXLIB)
	$(CC) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_sha1: test_sha1.c $(TXLIB)
	$(CC) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_sha256: test_sha256.c $(TXLIB)
	$(CC) $(CPPFLAGS) $@.c -o $@ $(TXLIB)

test_sha512: test_sha512.c $(TXLIB)
	$(CC) $(CPPFLAGS) $@.c -o $@ $(TXLIB)


clean:
	rm -f $(OBJS) *.o *.out *.ver *.log
	rm -f bigint_ex keypair keypair_ex leveldb_test leveldbex \
		rapidjson_ex xsz_ex speed xsz_ex \
		zmq_cli zmq_msvr zmq_svr \
		test_aes test_base58 test_base64 test_hashlib test_md5 \
		test_rc4 test_sha1 test_sha256 test_sha512
	rm -rf level.db
