CC	= g++ -O
INCLUDE	= -I../lib
CFLAGS	= -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter -fno-strict-aliasing

all:	hashlib.a test

hashlib.a: md5.o rc4.o sha1.o sha256.o sha512.o base58.o base64.o libhash.o aes.o
	rm -f $@
	ar cru $@ md5.o rc4.o sha1.o sha256.o sha512.o base58.o base64.o libhash.o aes.o

test: test_hashlib test_base58 test_base64 test_md5 test_rc4 \
	test_sha1 test_sha256 test_sha512 test_aes


test_hashlib: test_hashlib.o hashlib.a
	$(CC) $(CFLAGS) $@.c -o $@ hashlib.a

test_base58: test_base58.o hashlib.a
	$(CC) $(CFLAGS) $@.c -o $@ hashlib.a

test_base64: test_base64.o hashlib.a
	$(CC) $(CFLAGS) $@.c -o $@ hashlib.a

test_md5: test_md5.o hashlib.a
	$(CC) $(CFLAGS) $@.c -o $@ hashlib.a

test_rc4: test_rc4.o hashlib.a
	$(CC) $(CFLAGS) $@.c -o $@ hashlib.a

test_sha1: test_sha1.o hashlib.a
	$(CC) $(CFLAGS) $@.c -o $@ hashlib.a

test_sha256: test_sha256.o hashlib.a
	$(CC) $(CFLAGS) $@.c -o $@ hashlib.a

test_sha512: test_sha512.o hashlib.a
	$(CC) $(CFLAGS) $@.c -o $@ hashlib.a

test_aes: test_aes.o aes.o ../lib/xsz.cpp
	$(CC) $(CFLAGS) $@.c aes.o ../lib/xsz.cpp -o $@ 


clean:
	rm -f *.o hashlib.a
	rm -f test_aes test_base58 test_base64 test_hashlib test_md5 test_rc4 test_sha1 \
	test_sha256 test_sha512
