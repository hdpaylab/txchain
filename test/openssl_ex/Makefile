CPP	= g++

test: key.cpp myrsa.cpp main.cpp
	$(CPP) key.cpp myrsa.cpp main.cpp -o test -lcrypto

clean:
	rm -f test _decrypted.txt _encrypted.txt _signture.txt pri.pem private.pem pub.pem public.pem 
