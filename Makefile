CC	= g++

CSDKDIR	= HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -IHdacCSDK2/include -Ilib

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter

HDACLIB	= ../HdacCSDK2/hdaclib.a


all: lib test

lib: $(HDACLIB) txlib.a

txlib.a:
	cd lib; make; cd ..

$(HDACLIB): 
	cd $(CSDKDIR); make; cd ..

test:
	cd test; make; cd ..

bak:
	tar cf ~/BAK/txchain-`date +"%y%m%d"`.tar M* README* HISTORY \
		*/*.[hc]* */Make* */README* */*/*.[hc]* */*/Make* */*/*/*.[hc]* 
	gzip -f ~/BAK/txchain-`date +"%y%m%d"`.tar

cleanall:
	cd $(CSDKDIR); make clean; cd ..
	cd lib; make clean; cd ..
	make clean

clean:
	rm -f lib/*.o
	cd v0.1; make clean; cd ..
	cd v0.2; make clean; cd ..
	cd v0.3; make clean; cd ..
	cd test; make clean; cd ..
