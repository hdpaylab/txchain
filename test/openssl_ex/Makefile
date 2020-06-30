CPP	= g++

test: rsakey.cpp xrsa.cpp main.cpp
	$(CPP) rsakey.cpp xrsa.cpp main.cpp -o test -lcrypto

clean:
	rm -f test _decrypted.txt _encrypted.txt _signture.txt _evp_decrypt.txt _evp_encrypt.txt \
		pri.pem private.pem pub.pem public.pem 
