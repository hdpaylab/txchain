all:	rsa_lib rsa

rsa_lib: rsa_lib.o
	gcc rsa_lib.c -c -lcrypto -lssl -I./

rsa:	rsa.o
	gcc -o rsa rsa.c rsa_lib.o -lcrypto -lssl -I./

clean:
	rm -f *.o rsa
